#include "Message/MessageStackWidget.h"

#include "Components/PanelWidget.h"

bool UMessageStackWidget::Initialize()
{
    if (Super::Initialize())
    {
        // Init timestamp widget
        if (Messages.Num() > 0)
        {
            Timestamp->Message = Messages.Last();
            Timestamp->Side = Side;
        }
        return true;
    }

    return false;
}

void UMessageStackWidget::NativePreConstruct()
{
    // Spawn bubbles
    TextBubblePanel->ClearChildren();

    const int32 LastIndex = Messages.Num() - 1;
    for (int32 Index = 0; Index < Messages.Num(); ++Index)
    {
        const FMessage& Message = Messages[Index];
        UTextBubbleWidget* Widget = CreateWidget<UTextBubbleWidget>(this, TextBubbleWidgetClass);
        Widget->Message = Message;
        Widget->Side = Side;
        Widget->Position = Index == LastIndex ? EBubbleStackPosition::End : EBubbleStackPosition::Opening;
        TextBubblePanel->AddChild(Widget);
    }

    Super::NativePreConstruct();
}
