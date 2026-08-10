// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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
                                 .ListItemsSource(&Items)
                                 .CreateListViewWidget_UObject(this, &UMessageListWidget::CreateMessageWidget)
                                 .OnPaginating_Lambda([this](const EPaginationDirection Direction, const EHttpRequestState State)
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
        GetChannel()->RepliesUpdated.AddDynamic(this, &UMessageListWidget::OnRepliesUpdated);
        GetChannel()->MessageSent.AddDynamic(this, &UMessageListWidget::ScrollToBottom);
    }
    if (GetChannelContext())
    {
        GetChannelContext()->OnStartEditMessage.AddDynamic(this, &UMessageListWidget::ScrollToBottom);
        GetChannelContext()->OnThreadChanged.AddDynamic(this, &UMessageListWidget::OnThreadChanged);
    }
}

void UMessageListWidget::NativeDestruct()
{
    if (GetChannel())
    {
        GetChannel()->MessagesUpdated.RemoveDynamic(this, &UMessageListWidget::OnMessagesUpdated);
        GetChannel()->RepliesUpdated.RemoveDynamic(this, &UMessageListWidget::OnRepliesUpdated);
        GetChannel()->MessageSent.RemoveDynamic(this, &UMessageListWidget::ScrollToBottom);
    }
    if (GetChannelContext())
    {
        GetChannelContext()->OnStartEditMessage.RemoveDynamic(this, &UMessageListWidget::ScrollToBottom);
        GetChannelContext()->OnThreadChanged.RemoveDynamic(this, &UMessageListWidget::OnThreadChanged);
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
    if (!GetChannel())
    {
        Callback();
        return;
    }

    if (IsThreadOpen())
    {
        // Scrolling up a thread pages back through its replies, not the channel behind it
        GetChannel()->QueryAdditionalReplies(GetChannelContext()->GetThreadParentMessage(), Limit, [Callback](const TArray<FMessage>&) { Callback(); });
        return;
    }

    GetChannel()->QueryAdditionalMessages(PaginationDirection, Limit, Callback);
}

void UMessageListWidget::OnMessagesUpdated()
{
    RefreshItems();

    if (PaginateListWidget.IsValid())
    {
        // TODO this could likely be further optimized if we dig into WidgetGenerator
        PaginateListWidget->RebuildList();
    }

    // Reading a thread says nothing about the rest of the channel, so the channel is only marked
    // read while its own message list is what's on screen
    if (GetChannel() && !IsThreadOpen())
    {
        GetChannel()->MarkRead();
    }
}

void UMessageListWidget::OnRepliesUpdated(const FString& ParentMessageId)
{
    if (ParentMessageId != ThreadParentId)
    {
        // A reply arrived in some other thread. It changes that thread's reply count, which the
        // channel list draws under the parent, and MessagesUpdated has already covered that.
        return;
    }

    OnMessagesUpdated();
}

void UMessageListWidget::OnThreadChanged(const bool bThreadOpen, const FMessage& ParentMessage)
{
    ThreadParentId = bThreadOpen ? ParentMessage.Id : FString{};

    OnMessagesUpdated();

    if (PaginateListWidget.IsValid())
    {
        // Both directions are a fresh list to the user, so start at the newest message either way
        PaginateListWidget->ScrollToBottom();
    }
}

bool UMessageListWidget::IsThreadOpen() const
{
    return !ThreadParentId.IsEmpty() && GetChannelContext() && GetChannelContext()->IsThreadOpen();
}

void UMessageListWidget::RefreshItems()
{
    Items.Reset();

    UChatChannel* Channel = GetChannel();
    if (!Channel)
    {
        return;
    }

    if (!IsThreadOpen())
    {
        Items = Channel->State.Messages.GetMessages();
        return;
    }

    const FMessage& Parent = GetChannelContext()->GetThreadParentMessage();
    const FMessages& ChannelMessages = Channel->State.Messages.GetMessages();

    // The parent heads the thread, so the user can see what is being replied to. Preferring the copy
    // the channel holds keeps it live: a reaction or an edit on the parent shows up here too.
    const int32 ParentIndex = ChannelMessages.IndexOfByPredicate([&Parent](const FMessageRef& M) { return M->Id == Parent.Id; });
    Items.Add(ParentIndex != INDEX_NONE ? ChannelMessages[ParentIndex] : MakeShared<FMessage>(Parent));

    Items.Append(Channel->GetReplies(Parent));
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
    // Against what is on screen rather than what the store holds. In a thread the message after the
    // parent is its first reply, where the store would answer with the next message in the channel,
    // and the parent would lose the timestamp and avatar that end a stack.
    const int32 Index = Items.IndexOfByPredicate([&Message](const FMessageRef& M) { return M->Id == Message.Id; });
    const bool bIsLast = Index == INDEX_NONE || Index == Items.Num() - 1;
    if (bIsLast || IsEndOfMessageStack(Message, *Items[Index + 1]))
    {
        return EMessagePosition::End;
    }
    return EMessagePosition::Opening;
}
