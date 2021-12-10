// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Message/MessageListWidget.h"

#include "Channel/ChatChannel.h"
#include "Context/ChannelContextWidget.h"
#include "UiBlueprintLibrary.h"

UMessageListWidget::UMessageListWidget()
{
    bWantsChannel = true;
}
void UMessageListWidget::CreateMessageStackWidgets(const TArray<FMessage>& Messages)
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
        if (Index == Last ||
            (Messages[Index + 1].CreatedAt - Messages[Index].CreatedAt > FTimespan::FromMinutes(1.) || Messages[Index + 1].User != Messages[Index].User))
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

void UMessageListWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (ScrollBox)
    {
        ScrollBox->OnUserScrolled.AddDynamic(this, &UMessageListWidget::OnUserScroll);
    }
}

void UMessageListWidget::OnChannel()
{
    Channel->MessagesUpdated.AddDynamic(this, &UMessageListWidget::SetMessages);
    Channel->MessageSent.AddDynamic(this, &UMessageListWidget::ScrollToBottom);

    ChannelContext->OnStartEditMessage.AddDynamic(this, &UMessageListWidget::ScrollToBottom);

    SetMessages(Channel->GetMessages());
}

void UMessageListWidget::SetMessages(const TArray<FMessage>& Messages)
{
    if (!ScrollBox)
    {
        return;
    }

    int32 FirstIndex;
    float FirstLeadingEdge;
    UUiBlueprintLibrary::GetFirstVisibleChildOfScrollBox(ScrollBox, FirstIndex, FirstLeadingEdge);
    const int32 StartListLength = ScrollBox->GetChildrenCount();

    CreateMessageStackWidgets(Messages);

    // Try to maintain the position of all children
    if (FirstIndex == INDEX_NONE)
    {
        ScrollBox->ScrollToEnd();
    }
    else
    {
        const int32 Index = ScrollBox->GetChildrenCount() - StartListLength + FirstIndex;
        UWidget* Widget = ScrollBox->GetChildAt(Index);
        ScrollBox->ScrollWidgetIntoView(Widget, false, EDescendantScrollDestination::TopOrLeft, FirstLeadingEdge);
    }
}

void UMessageListWidget::ScrollToBottom(const FMessage& Message)
{
    if (ScrollBox)
    {
        ScrollBox->ScrollToEnd();
    }
}

void UMessageListWidget::OnUserScroll(const float CurrentOffset)
{
    if (CurrentOffset < PaginateScrollThreshold)
    {
        if (Channel)
        {
            Channel->QueryAdditionalMessages();
        }
    }
}

void UMessageListWidget::NativeDestruct()
{
    if (Channel)
    {
        Channel->MessagesUpdated.RemoveDynamic(this, &UMessageListWidget::SetMessages);
        Channel->MessageSent.RemoveDynamic(this, &UMessageListWidget::ScrollToBottom);
    }

    if (ChannelContext)
    {
        ChannelContext->OnStartEditMessage.AddDynamic(this, &UMessageListWidget::ScrollToBottom);
    }
    Super::NativeDestruct();
}
