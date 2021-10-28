#include "Message/MessageStackWidget.h"

#include "Components/PanelWidget.h"

void UMessageStackWidget::NativePreConstruct()
{
    TextBubblePanel->ClearChildren();

    const int32 LastIndex = Messages.Num() - 1;
    for (int32 Index = 0; Index < Messages.Num(); ++Index)
    {
        const FMessage& Message = Messages[Index];
        UTextBubbleWidget* Widget = CreateWidget<UTextBubbleWidget>(this, TextBubbleWidgetClass);
        Widget->Message = Message;
        Widget->Position = Index == LastIndex ? EBubbleStackPosition::End : EBubbleStackPosition::Opening;
        Widget->Side = Message.User.Id == CurrentUser.Id ? EBubbleStackSide::Me : EBubbleStackSide::You;
        TextBubblePanel->AddChild(Widget);
    }
    Super::NativePreConstruct();
}
