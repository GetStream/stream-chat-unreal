#include "Message/TextBubbleWidget.h"

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
        TextBlock->SetColorAndOpacity(GetTextColor());
    }

    if (Border)
    {
        Border->Background.Margin = {0.5f};
        Border->Background.DrawAs = ESlateBrushDrawType::Box;
        Border->SetBrushFromTexture(GetBubbleTexture());
        Border->SetBrushColor(GetBubbleColor());
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

const FLinearColor& UTextBubbleWidget::GetBubbleColor() const
{
    if (Message.Type == EMessageType::Deleted)
    {
        return DeletedBubbleColor;
    }
    if (Side == EMessageSide::Me)
    {
        return MeBubbleColor;
    }
    return FLinearColor::White;
}

const FLinearColor& UTextBubbleWidget::GetTextColor() const
{
    if (Message.Type == EMessageType::Deleted)
    {
        return DeletedTextColor;
    }
    return NormalTextColor;
}

FText UTextBubbleWidget::GetText() const
{
    if (Message.Type == EMessageType::Deleted)
    {
        return FText::FromString(TEXT("Message deleted"));
    }
    return FText::FromString(Message.Text);
}
