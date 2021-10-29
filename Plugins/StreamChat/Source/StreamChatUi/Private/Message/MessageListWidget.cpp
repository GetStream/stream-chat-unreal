#include "Message/MessageListWidget.h"

#include "ChannelContextWidget.h"

void UMessageListWidget::CreateMessageStackWidgets(const TArray<FMessage> Messages)
{
    ScrollBox->ClearChildren();

    const FString CurrentUserId = Channel->CurrentUser.Id;

    // A new stack is formed when:
    // 1. A minute is passed between 2 messages sent from the same user.
    // 2. Another users sends a message.
    // 3. A date stamp appears. (TODO)
    int32 StartIndex = 0;
    const int32 Last = Messages.Num() - 1;
    for (int32 Index = 0; Index < Messages.Num(); ++Index)
    {
        if (Index == Last ||
            (Index != 0 && (Messages[Index].CreatedAt - Messages[Index - 1].CreatedAt > FTimespan::FromMinutes(1.) ||
                            Messages[Index].User.Id != Messages[Index - 1].User.Id)))
        {
            const int32 Count = Index - StartIndex;

            UMessageStackWidget* Widget = CreateWidget<UMessageStackWidget>(this, MessageStackWidgetClass);
            TArray<FMessage> StackMessages;
            StackMessages.Reserve(Count);
            StackMessages.Append(&Messages[StartIndex], Count);
            Widget->Setup(
                StackMessages, Messages[Index].User.Id == CurrentUserId ? EBubbleStackSide::Me : EBubbleStackSide::You);

            ScrollBox->AddChild(Widget);

            StartIndex = Index;
        }
    }
}
