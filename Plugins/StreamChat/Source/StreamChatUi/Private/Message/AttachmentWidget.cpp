// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Message/AttachmentWidget.h"

#include "AttachmentLayout.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Engine.h"
#include "Engine/Texture2DDynamic.h"
#include "ImageDownloadSubsystem.h"
#include "ThemeDataAsset.h"

namespace
{
/// Last path segment of a URL, without the query string
FString FileNameFromUrl(const FString& Url)
{
    FString Path;
    if (!Url.Split(TEXT("?"), &Path, nullptr))
    {
        Path = Url;
    }

    FString FileName;
    if (Path.Split(TEXT("/"), nullptr, &FileName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
    {
        return FileName;
    }
    return Path;
}

void SetFontSize(UTextBlock& TextBlock, const int32 Size)
{
    FSlateFontInfo Font = TextBlock.GetFont();
    Font.Size = Size;
    TextBlock.SetFont(Font);
}
}    // namespace

void UAttachmentWidget::Setup(const FAttachment& InAttachment, const EMessageSide InSide)
{
    Attachment = InAttachment;
    Side = InSide;

    Super::Setup();
}

void UAttachmentWidget::OnSetup()
{
    UPanelWidget* Panel = GetOrCreateContentPanel();
    if (!Panel || !WidgetTree)
    {
        return;
    }

    // Setup can run more than once: UStreamWidget::Initialize calls it with whatever the widget was
    // constructed with, before the caller has had the chance to hand over an attachment.
    Panel->ClearChildren();
    bImageLoaded = false;

    const bool bInline = ShouldRenderInline();

    Image = WidgetTree->ConstructWidget<UImage>();
    ImageSizer = WidgetTree->ConstructWidget<USizeBox>();
    ImageSizer->AddChild(Image);
    ImageSizer->SetVisibility(bInline ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
    Panel->AddChild(ImageSizer);

    // The backend reports the original dimensions, so the right amount of room can be reserved before
    // the image arrives and the message list does not jump when it does
    const TOptional<int32> SourceWidth = Attachment.ExtraData.GetNumber<int32>(TEXT("original_width"));
    const TOptional<int32> SourceHeight = Attachment.ExtraData.GetNumber<int32>(TEXT("original_height"));
    ApplyImageSize(GetImageDisplaySize(SourceWidth.Get(0), SourceHeight.Get(0)));

    FileRow = WidgetTree->ConstructWidget<UBorder>();
    FileRow->SetPadding(FileRowPadding);
    FileRow->SetVisibility(bInline ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
    Panel->AddChild(FileRow);

    UVerticalBox* Lines = WidgetTree->ConstructWidget<UVerticalBox>();
    FileRow->SetContent(Lines);

    TitleTextBlock = WidgetTree->ConstructWidget<UTextBlock>();
    TitleTextBlock->SetText(FText::FromString(GetTitle()));
    TitleTextBlock->SetAutoWrapText(true);
    // A hard wrap width rather than a size box: auto wrapping inside one measures the unwrapped text
    // for the desired height and then clips whatever the wrap pushes onto a second line.
    TitleTextBlock->SetWrapTextAt(GetImageBudgetWidth());
    Lines->AddChild(TitleTextBlock);

    const FString Subtitle = GetSubtitle();
    SubtitleTextBlock = WidgetTree->ConstructWidget<UTextBlock>();
    SubtitleTextBlock->SetText(FText::FromString(Subtitle));
    SubtitleTextBlock->SetVisibility(Subtitle.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
    Lines->AddChild(SubtitleTextBlock);

    if (bInline)
    {
        FetchRemoteImage();
    }
}

void UAttachmentWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    const UThemeDataAsset* Theme = GetTheme();
    if (!Theme)
    {
        return;
    }

    if (Image && !bImageLoaded)
    {
        // Tint the empty brush so a downloading image reads as a placeholder rather than a gap
        Image->SetColorAndOpacity(Theme->GetPaletteColor(Theme->AttachmentImagePlaceholderColor));
    }

    if (FileRow)
    {
        const FName Color = Side == EMessageSide::Me ? Theme->MeAttachmentRowColor : Theme->YouAttachmentRowColor;
        FileRow->SetBrushColor(Theme->GetPaletteColor(Color));
    }

    if (TitleTextBlock)
    {
        TitleTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->AttachmentTitleTextColor));
        SetFontSize(*TitleTextBlock, TitleFontSize);
    }

    if (SubtitleTextBlock)
    {
        SubtitleTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->AttachmentSubtitleTextColor));
        SetFontSize(*SubtitleTextBlock, SubtitleFontSize);
    }
}

bool UAttachmentWidget::ShouldRenderInline() const
{
    const bool bIsPicture = Attachment.Type == EAttachmentType::Image || Attachment.Type == EAttachmentType::Giphy;
    return bIsPicture && !Attachment.GetUrl().IsEmpty();
}

void UAttachmentWidget::FetchRemoteImage()
{
    UImageDownloadSubsystem* Subsystem = GEngine ? GEngine->GetEngineSubsystem<UImageDownloadSubsystem>() : nullptr;
    if (!Subsystem)
    {
        return;
    }

    FString Url = Attachment.GetUrl();
    if (bRequestResizedImages)
    {
        // Slate units times the DPI factor gives pixels, and the box is allowed to be taller than it
        // is wide so a portrait photo still comes back fitted to its width
        const int32 WidthPixels = FMath::CeilToInt(GetImageBudgetWidth() * GetDpiScale());
        Url = AttachmentLayout::WithResizeQuery(Url, FIntPoint{WidthPixels, FMath::CeilToInt(WidthPixels * MaxImageAspect)});
    }

    Subsystem->DownloadImage(
        Url,
        [WeakThis = TWeakObjectPtr<UAttachmentWidget>(this)](UTexture2DDynamic* Texture)
        {
            if (WeakThis.IsValid())
            {
                WeakThis->OnImageDownloaded(Texture);
            }
        });
}

void UAttachmentWidget::OnImageDownloaded(UTexture2DDynamic* Texture)
{
    if (!Image || !FileRow || !ImageSizer)
    {
        return;
    }

    if (!Texture)
    {
        // Nothing to draw, so fall back to naming the file like any other attachment
        ImageSizer->SetVisibility(ESlateVisibility::Collapsed);
        FileRow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        return;
    }

    bImageLoaded = true;
    // bMatchSize would set the brush to the source's pixel dimensions, which is not what it gets
    // drawn at; the size box decides that, and the brush is told to agree
    Image->SetBrushFromTextureDynamic(Texture, false);
    Image->SetColorAndOpacity(FLinearColor::White);
    ApplyImageSize(GetImageDisplaySize(Texture->SizeX, Texture->SizeY));
    Invalidate(EInvalidateWidgetReason::LayoutAndVolatility);
}

float UAttachmentWidget::GetAvailableWidth() const
{
    // No world means no viewport to measure, which is the case in an offline test
    if (!GetWorld())
    {
        return MaxImageWidth;
    }

    const float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
    const FVector2D Viewport = UWidgetLayoutLibrary::GetViewportSize(this);
    if (Scale <= 0.f || Viewport.X <= 1.f)
    {
        return MaxImageWidth;
    }

    // Slate units, so the fraction means the same thing on a handset and on a desktop
    return static_cast<float>(Viewport.X) / Scale;
}

float UAttachmentWidget::GetImageBudgetWidth() const
{
    return FMath::Min(GetAvailableWidth() * ImageWidthFraction, MaxImageWidth);
}

FVector2D UAttachmentWidget::GetImageDisplaySize(const int32 SourceWidth, const int32 SourceHeight) const
{
    return AttachmentLayout::FitToBudget(FIntPoint{SourceWidth, SourceHeight}, GetImageBudgetWidth(), MaxImageAspect, GetDpiScale());
}

float UAttachmentWidget::GetDpiScale() const
{
    return GetWorld() ? FMath::Max(1.f, UWidgetLayoutLibrary::GetViewportScale(this)) : 1.f;
}

void UAttachmentWidget::ApplyImageSize(const FVector2D& Size)
{
    if (!ImageSizer)
    {
        return;
    }

    // The size box alone decides the geometry. The brush's own size only feeds the desired size,
    // which the overrides above replace, so there is nothing to keep in step with it.
    ImageSizer->SetWidthOverride(static_cast<float>(Size.X));
    ImageSizer->SetHeightOverride(static_cast<float>(Size.Y));
}

FString UAttachmentWidget::GetTitle() const
{
    if (!Attachment.Title.IsEmpty())
    {
        return Attachment.Title;
    }
    if (!Attachment.Text.IsEmpty())
    {
        return Attachment.Text;
    }

    const FString FileName = FileNameFromUrl(Attachment.GetUrl());
    return FileName.IsEmpty() ? TEXT("Attachment") : FileName;
}

FString UAttachmentWidget::GetSubtitle() const
{
    TArray<FString> Parts;
    if (!Attachment.MimeType.IsEmpty())
    {
        Parts.Add(Attachment.MimeType);
    }
    if (Attachment.FileSize > 0)
    {
        Parts.Add(FText::AsMemory(Attachment.FileSize).ToString());
    }
    return FString::Join(Parts, TEXT(" · "));
}

UPanelWidget* UAttachmentWidget::GetOrCreateContentPanel()
{
    if (ContentPanel)
    {
        return ContentPanel;
    }

    if (!WidgetTree)
    {
        return nullptr;
    }

    // No widget blueprint bound a panel, so this widget is the whole tree
    UVerticalBox* Generated = WidgetTree->ConstructWidget<UVerticalBox>();
    WidgetTree->RootWidget = Generated;
    ContentPanel = Generated;
    return ContentPanel;
}
