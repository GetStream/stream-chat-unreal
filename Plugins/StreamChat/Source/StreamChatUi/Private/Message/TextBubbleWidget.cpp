// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Message/TextBubbleWidget.h"

#include "ThemeDataAsset.h"

UTextBubbleWidget::UTextBubbleWidget()
{
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
    if (TextBlock)
    {
        TextBlock->SetText(GetText());
    }
}

void UTextBubbleWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    const bool bSingleEmoji = IsSingleEmoji();
    if (TextBlock)
    {
        TextBlock->SetHtmlStyles(GetTheme()->BubbleHtmlStyles);

        if (Message.Type == EMessageType::Deleted)
        {
            TextBlock->SetDefaultColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->DeletedMessageTextColor));
        }
        else if (bSingleEmoji)
        {
            TextBlock->SetDefaultFont(SingleEmojiFont);
        }
        else
        {
            TextBlock->ClearAllDefaultStyleOverrides();
        }
    }

    if (Border)
    {
        Border->SetBrushColor(GetBubbleColor());

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

const FLinearColor& UTextBubbleWidget::GetBubbleColor() const
{
    if (Message.Type == EMessageType::Deleted)
    {
        return GetTheme()->GetPaletteColor(GetTheme()->DeletedBubbleColor);
    }
    if (Side == EMessageSide::Me)
    {
        return GetTheme()->GetPaletteColor(GetTheme()->MeBubbleColor);
    }
    return GetTheme()->GetPaletteColor(GetTheme()->YouBubbleColor);
}

FText UTextBubbleWidget::GetText() const
{
    if (Message.Type == EMessageType::Deleted)
    {
        return FText::FromString(TEXT("Message deleted"));
    }
    if (!Message.Html.IsEmpty())
    {
        return FText::FromString(Message.Html);
    }
    return FText::FromString(Message.Text);
}

bool UTextBubbleWidget::IsSingleEmoji() const
{
    const TSharedRef<FSlateFontCache> FontCache = FSlateApplication::Get().GetRenderer()->GetFontCache();
    const FShapedGlyphSequenceRef Shaped =
        FontCache->ShapeUnidirectionalText(Message.Text, SingleEmojiFont, 1.f, TextBiDi::ETextDirection::LeftToRight, ETextShapingMethod::Auto);
    const bool bIsSingleMulticharacterGlyph = Shaped->GetGlyphsToRender().Num() == 1 && Shaped->GetGlyphsToRender()[0].NumCharactersInGlyph > 1;
    return bIsSingleMulticharacterGlyph;
}
