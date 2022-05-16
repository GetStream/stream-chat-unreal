// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Message/MessageListWidget.h"

#include "Channel/ChatChannel.h"
#include "Context/ChannelContextWidget.h"

namespace
{
bool IsEndOfMessageStack(const FMessage& CurrentMessage, const FMessage& NextMessage)
{
    // A new stack is formed when:
    // 1. A minute is passed between 2 messages sent from the same user.
    // 2. Another users sends a message.
    // TODO 3. A date stamp appears.
    if (NextMessage.CreatedAt - CurrentMessage.CreatedAt > FTimespan::FromMinutes(1.))
    {
        return true;
    }
    if (NextMessage.User != CurrentMessage.User)
    {
        return true;
    }
    return false;
}

EMessageSide GetSide(const FMessage& Message)
{
    return Message.User.IsCurrent() ? EMessageSide::Me : EMessageSide::You;
}

}    // namespace

UMessageListWidget::UMessageListWidget() : ListViewGenerator(MakeShared<TListViewGenerator<FMessageRef>>())
{
    bWantsChannel = true;
    PaginationDirection = EPaginationDirection::Top;
}

void UMessageListWidget::OnChannel()
{
    Channel->MessagesUpdated.AddDynamic(this, &UMessageListWidget::OnMessagesUpdated);
    Channel->MessageSent.AddDynamic(this, &UMessageListWidget::ScrollToBottom);

    ChannelContext->OnStartEditMessage.AddDynamic(this, &UMessageListWidget::ScrollToBottom);

    OnMessagesUpdated();
}

void UMessageListWidget::NativeDestruct()
{
    if (Channel)
    {
        Channel->MessagesUpdated.RemoveDynamic(this, &UMessageListWidget::OnMessagesUpdated);
        Channel->MessageSent.RemoveDynamic(this, &UMessageListWidget::ScrollToBottom);
    }

    if (ChannelContext)
    {
        ChannelContext->OnStartEditMessage.RemoveDynamic(this, &UMessageListWidget::ScrollToBottom);
    }
    Super::NativeDestruct();
}

void UMessageListWidget::CreateListView()
{
    if (!Channel || !ListView)
    {
        return;
    }

    ListViewGenerator->CreateListView(
        &Channel->State.Messages.GetMessages(),
        TListViewGenerator<FMessageRef>::FCreateListViewWidgetDelegate::CreateUObject(this, &UMessageListWidget::CreateMessageWidget),
        ListView);
}

void UMessageListWidget::Paginate(const EPaginationDirection Direction, const TFunction<void()> Callback)
{
    if (Channel)
    {
        Channel->QueryAdditionalMessages(PaginationDirection, Limit, Callback);
    }
    else
    {
        Callback();
    }
}

void UMessageListWidget::OnMessagesUpdated()
{
    if (!ScrollBox)
    {
        return;
    }

    CreateMessageWidgets();

    Channel->MarkRead();
}

void UMessageListWidget::CreateMessageWidgets()
{
    if (!ScrollBox)
    {
        return;
    }

    TArray<UWidget*> Widgets;
    const FMessages& Messages = Channel->State.Messages.GetMessages();
    Widgets.Reserve(Messages.Num());

    for (auto&& Message : Messages)
    {
        UWidget* Widget = CreateMessageWidget(Message);
        Widgets.Add(Widget);
    }
    SetChildren(Widgets);
}

UWidget* UMessageListWidget::CreateMessageWidget(const FMessageRef& Message)
{
    const EMessageSide Side = GetSide(*Message);
    const EMessagePosition Position = GetPosition(*Message);
    if (OnGetMessageWidgetEvent.IsBound())
    {
        if (UMessageWidget* Widget = OnGetMessageWidgetEvent.Execute(*Message, Side, Position))
        {
            return Widget;
        }
    }

    UMessageWidget* Widget = CreateWidget<UMessageWidget>(this, MessageWidgetClass);
    Widget->Setup(*Message, Side, Position);
    return Widget;
}

void UMessageListWidget::ScrollToBottom(const FMessage&)
{
    if (ScrollBox)
    {
        ScrollBox->ScrollToEnd();
    }
}

EMessagePosition UMessageListWidget::GetPosition(const FMessage& Message) const
{
    const TSharedPtr<FMessage> NextMessage = Channel->State.Messages.Next(Message);
    if (!NextMessage.IsValid() || IsEndOfMessageStack(Message, *NextMessage))
    {
        return EMessagePosition::End;
    }
    return EMessagePosition::Opening;
}
