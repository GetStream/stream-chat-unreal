#include "Message/TextBubbleWidget.h"

void UTextBubbleWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    TextBlock->SetText(FText::FromString(Message.Text));
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

FLinearColor UTextBubbleWidget::GetBubbleColor() const
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

FLinearColor UTextBubbleWidget::GetTextColor() const
{
    if (Message.DeletedAt.IsSet())
    {
        return DeletedTextColor;
    }
    return NormalTextColor;
}
