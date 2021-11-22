#include "Message/MessageListWidget.h"

#include "Channel/ChatChannel.h"

void UMessageListWidget::CreateMessageStackWidgets(const TArray<FMessage> Messages)
{
    if (!ScrollBox)
    {
        return;
    }

    ScrollBox->ClearChildren();

    // A new stack is formed when:
    // 1. A minute is passed between 2 messages sent from the same user.
    // 2. Another users sends a message.
    // TODO 3. A date stamp appears.
    int32 StartIndex = 0;
    const int32 Last = Messages.Num() - 1;
    for (int32 Index = 0; Index < Messages.Num(); ++Index)
    {
        if (Index == Last || (Messages[Index + 1].CreatedAt - Messages[Index].CreatedAt > FTimespan::FromMinutes(1.) ||
                              Messages[Index + 1].User != Messages[Index].User))
        {
            const int32 Count = Index + 1 - StartIndex;

            UMessageStackWidget* Widget = CreateWidget<UMessageStackWidget>(this, MessageStackWidgetClass);
            TArray<FMessage> StackMessages;
            StackMessages.Reserve(Count);
            StackMessages.Append(&Messages[StartIndex], Count);
            Widget->Setup(StackMessages, Messages[Index].User.IsCurrent() ? EMessageSide::Me : EMessageSide::You);

            ScrollBox->AddChild(Widget);

            StartIndex = Index + 1;
        }
    }
}
