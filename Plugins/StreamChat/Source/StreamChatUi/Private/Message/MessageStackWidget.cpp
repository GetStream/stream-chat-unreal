#include "Message/MessageStackWidget.h"

#include "Components/PanelWidget.h"
#include "Message/MessageWidget.h"

bool UMessageStackWidget::Initialize()
{
    if (Super::Initialize())
    {
        if (IsDesignTime())
        {
            SetupChildren();
        }
        return true;
    }

    return false;
}

void UMessageStackWidget::Setup(const TArray<FMessage>& InMessages, const EBubbleStackSide InSide)
{
    Messages = InMessages;
    Side = InSide;

    SetupChildren();
}

void UMessageStackWidget::SetupChildren()
{
    // Init timestamp widget
    if (Messages.Num() > 0)
    {
        Timestamp->Setup(Messages.Last(), Side);
    }
}

void UMessageStackWidget::NativePreConstruct()
{
    // Spawn bubbles
    MessagesPanel->ClearChildren();

    const int32 LastIndex = Messages.Num() - 1;
    for (int32 Index = 0; Index < Messages.Num(); ++Index)
    {
        const FMessage& Message = Messages[Index];
        UMessageWidget* Widget = CreateWidget<UMessageWidget>(this, MessageWidgetClass);
        Widget->Setup(Message, Side, Index == LastIndex ? EBubbleStackPosition::End : EBubbleStackPosition::Opening);
       MessagesPanel->AddChildToVerticalBox(Widget);
    }

    Super::NativePreConstruct();
}
