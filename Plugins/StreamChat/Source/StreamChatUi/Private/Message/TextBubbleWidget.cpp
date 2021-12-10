// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Message/TextBubbleWidget.h"

#include "ThemeDataAsset.h"

UTextBubbleWidget::UTextBubbleWidget()
{
    bWantsTheme = true;
}

void UTextBubbleWidget::Setup(const FMessage& InMessage, EMessageSide InSide, EBubbleStackPosition InPosition)
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

    if (Border)
    {
        Border->Background.Margin = {0.5f};
        Border->Background.DrawAs = ESlateBrushDrawType::Box;
        Border->SetBrushFromTexture(GetBubbleTexture());
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
        case EBubbleStackPosition::Opening:
        {
            switch (Side)
            {
                case EMessageSide::You:
                    return YouOpeningTexture;
                case EMessageSide::Me:
                    return MaskOpeningTexture;
            }
        }
        case EBubbleStackPosition::End:
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
