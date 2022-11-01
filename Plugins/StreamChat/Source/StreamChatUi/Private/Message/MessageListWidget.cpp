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

UMessageListWidget::UMessageListWidget()
{
}

void UMessageListWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (GetChannel() && ListView)
    {
        PaginateListWidget = SNew(SPaginateListWidget<FMessageRef>)
                                 .Limit(Limit)
                                 .PaginationDirection(EPaginationDirection::Top)
                                 .ListItemsSource(&GetChannel()->State.Messages.GetMessages())
                                 .CreateListViewWidget_UObject(this, &UMessageListWidget::CreateMessageWidget)
                                 .OnPaginating_Lambda([=](const EPaginationDirection Direction, const EHttpRequestState State)
                                                      { OnPaginatingMessages.Broadcast(Direction, State); })
                                 .DoPaginate_UObject(this, &UMessageListWidget::Paginate);
        ListView->SetContent(PaginateListWidget.ToSharedRef());
    }

    OnMessagesUpdated();
}

void UMessageListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (GetChannel())
    {
        GetChannel()->MessagesUpdated.AddDynamic(this, &UMessageListWidget::OnMessagesUpdated);
        GetChannel()->MessageSent.AddDynamic(this, &UMessageListWidget::ScrollToBottom);
    }
    if (GetChannelContext())
    {
        GetChannelContext()->OnStartEditMessage.AddDynamic(this, &UMessageListWidget::ScrollToBottom);
    }
}

void UMessageListWidget::NativeDestruct()
{
    if (GetChannel())
    {
        GetChannel()->MessagesUpdated.RemoveDynamic(this, &UMessageListWidget::OnMessagesUpdated);
        GetChannel()->MessageSent.RemoveDynamic(this, &UMessageListWidget::ScrollToBottom);
    }
    if (GetChannelContext())
    {
        GetChannelContext()->OnStartEditMessage.RemoveDynamic(this, &UMessageListWidget::ScrollToBottom);
    }
    Super::NativeDestruct();
}

void UMessageListWidget::ReleaseSlateResources(const bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);

    PaginateListWidget.Reset();
}

void UMessageListWidget::Paginate(const EPaginationDirection PaginationDirection, const TFunction<void()> Callback)
{
    if (GetChannel())
    {
        GetChannel()->QueryAdditionalMessages(PaginationDirection, Limit, Callback);
    }
    else
    {
        Callback();
    }
}

void UMessageListWidget::OnMessagesUpdated()
{
    if (PaginateListWidget.IsValid())
    {
        // TODO this could likely be further optimized if we dig into WidgetGenerator
        PaginateListWidget->RebuildList();
    }

    if (GetChannel())
    {
        GetChannel()->MarkRead();
    }
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
    if (PaginateListWidget.IsValid())
    {
        PaginateListWidget->ScrollToBottom();
    }
}

EMessagePosition UMessageListWidget::GetPosition(const FMessage& Message) const
{
    const TSharedPtr<FMessage> NextMessage = GetChannel()->State.Messages.Next(Message);
    if (!NextMessage.IsValid() || IsEndOfMessageStack(Message, *NextMessage))
    {
        return EMessagePosition::End;
    }
    return EMessagePosition::Opening;
}
