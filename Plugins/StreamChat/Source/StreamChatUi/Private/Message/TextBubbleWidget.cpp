// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Message/TextBubbleWidget.h"

#include "ThemeDataAsset.h"

UTextBubbleWidget::UTextBubbleWidget()
{
    bWantsTheme = true;
}

void UTextBubbleWidget::Setup(const FMessage& InMessage, const EMessageSide InSide, EMessagePosition InPosition)
{
    Message = InMessage;
    Side = InSide;
    Position = InPosition;

    Super::Setup();
}

void UTextBubbleWidget::OnSetup()
{
    const bool bSingleEmoji = IsSingleEmoji();
    if (TextBlock)
    {
        TextBlock->SetText(GetText());

        if (bSingleEmoji)
        {
            FSlateFontInfo LargeFont{NormalFont};
            LargeFont.Size = SingleEmojiFontSize;
            TextBlock->SetFont(LargeFont);
        }
        else
        {
            TextBlock->SetFont(NormalFont);
        }
    }

    if (Border)
    {
        if (bSingleEmoji)
        {
            FSlateBrush None{};
            None.DrawAs = ESlateBrushDrawType::NoDrawType;
            Border->SetBrush(None);
            Border->SetPadding({0.f});
        }
        else
        {
            Border->Background.Margin = {0.5f};
            Border->Background.DrawAs = ESlateBrushDrawType::Box;
            Border->SetBrushFromTexture(GetBubbleTexture());
        }
    }
}

void UTextBubbleWidget::OnTheme(const UThemeDataAsset* Theme)
{
    if (TextBlock)
    {
        TextBlock->SetColorAndOpacity(GetTextColor(Theme));
    }
    if (Border)
    {
        Border->SetBrushColor(GetBubbleColor(Theme));
    }
}

UTexture2D* UTextBubbleWidget::GetBubbleTexture() const
{
    switch (Position)
    {
        case EMessagePosition::Opening:
        {
            switch (Side)
            {
                case EMessageSide::You:
                    return YouOpeningTexture;
                case EMessageSide::Me:
                    return MaskOpeningTexture;
            }
        }
        case EMessagePosition::End:
        {
            switch (Side)
            {
                case EMessageSide::You:
                    return YouEndTexture;
                case EMessageSide::Me:
                    return MaskEndTexture;
            }
        }
    }
    return nullptr;
}

const FLinearColor& UTextBubbleWidget::GetBubbleColor(const UThemeDataAsset* Theme) const
{
    if (Message.Type == EMessageType::Deleted)
    {
        return Theme->GetPaletteColor(Theme->DeletedBubbleColor);
    }
    if (Side == EMessageSide::Me)
    {
        return Theme->GetPaletteColor(Theme->MeBubbleColor);
    }
    return Theme->GetPaletteColor(Theme->YouBubbleColor);
}

const FLinearColor& UTextBubbleWidget::GetTextColor(const UThemeDataAsset* Theme) const
{
    if (Message.Type == EMessageType::Deleted)
    {
        return Theme->GetPaletteColor(Theme->DeletedMessageTextColor);
    }
    return Theme->GetPaletteColor(Theme->NormalMessageTextColor);
}

FText UTextBubbleWidget::GetText() const
{
    if (Message.Type == EMessageType::Deleted)
    {
        return FText::FromString(TEXT("Message deleted"));
    }
    return FText::FromString(Message.Text);
}

bool UTextBubbleWidget::IsSingleEmoji() const
{
    const TSharedRef<FSlateFontCache> FontCache = FSlateApplication::Get().GetRenderer()->GetFontCache();
    const FShapedGlyphSequenceRef Shaped =
        FontCache->ShapeUnidirectionalText(Message.Text, NormalFont, 1.f, TextBiDi::ETextDirection::LeftToRight, ETextShapingMethod::Auto);
    const bool bIsSingleMulticharacterGlyph = Shaped->GetGlyphsToRender().Num() == 1 && Shaped->GetGlyphsToRender()[0].NumCharactersInGlyph > 1;
    return bIsSingleMulticharacterGlyph;
}
