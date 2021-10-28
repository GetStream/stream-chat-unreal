#include "Message/TextBubbleWidget.h"

void UTextBubbleWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    TextBlock->SetText(FText::FromString(Message.Text));

    Border->Background.Margin = {0.5f};
    Border->Background.DrawAs = ESlateBrushDrawType::Box;
    Border->SetBrushFromTexture(GetBorderTexture());
}

UTexture2D* UTextBubbleWidget::GetBorderTexture() const
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
                    return MeOpeningTexture;
            }
        }
        case EBubbleStackPosition::End:
        {
            switch (Side)
            {
                case EBubbleStackSide::You:
                    return YouEndTexture;
                case EBubbleStackSide::Me:
                    return MeEndTexture;
            }
        }
    }
    return nullptr;
}
