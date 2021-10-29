#include "Message/TextBubbleWidget.h"

void UTextBubbleWidget::Setup(const FMessage& InMessage, EBubbleStackSide InSide, EBubbleStackPosition InPosition)
{
    Message = InMessage;
    Side = InSide;
    Position = InPosition;
}

void UTextBubbleWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    TextBlock->SetText(GetText());
    TextBlock->SetColorAndOpacity(GetTextColor());

    Border->Background.Margin = {0.5f};
    Border->Background.DrawAs = ESlateBrushDrawType::Box;
    Border->SetBrushFromTexture(GetBubbleTexture());
    Border->SetBrushColor(GetBubbleColor());
}

UTexture2D* UTextBubbleWidget::GetBubbleTexture() const
{
    switch (Position)
    {
        case EBubbleStackPosition::Opening:
        {
            switch (Side)
            {
                case EBubbleStackSide::You:
                    return YouOpeningTexture;
                case EBubbleStackSide::Me:
                    return MaskOpeningTexture;
            }
        }
        case EBubbleStackPosition::End:
        {
            switch (Side)
            {
                case EBubbleStackSide::You:
                    return YouEndTexture;
                case EBubbleStackSide::Me:
                    return MaskEndTexture;
            }
        }
    }
    return nullptr;
}

const FLinearColor& UTextBubbleWidget::GetBubbleColor() const
{
    if (Message.DeletedAt.IsSet())
    {
        return DeletedBubbleColor;
    }
    if (Side == EBubbleStackSide::Me)
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
