// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"

#include "Algo/Transform.h"
#include "Blueprint/CallbackAction.h"
#include "Channel/Filter.h"
#include "ChatApi.h"
#include "Engine/World.h"
#include "Event/Channel/MessageDeletedEvent.h"
#include "Event/Channel/MessageNewEvent.h"
#include "Event/Channel/MessageReadEvent.h"
#include "Event/Channel/MessageUpdatedEvent.h"
#include "Event/Channel/ReactionDeletedEvent.h"
#include "Event/Channel/ReactionNewEvent.h"
#include "Event/Channel/ReactionUpdatedEvent.h"
#include "Event/Channel/TypingStartEvent.h"
#include "Event/Channel/TypingStopEvent.h"
#include "Event/Notification/NotificationMarkReadEvent.h"
#include "Event/Notification/NotificationMessageNewEvent.h"
#include "Reaction/Reaction.h"
#include "Request/Message/MessageRequestDto.h"
#include "Request/Reaction/ReactionRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/DeleteChannelResponseDto.h"
#include "Response/Message/MessageResponseDto.h"
#include "Response/Message/SearchResponseDto.h"
#include "TimerManager.h"
#include "User/UserManager.h"
#include "Util.h"

UChatChannel* UChatChannel::Create(
    UObject* Outer,
    const TSharedRef<FChatApi> Api,
    const TSharedRef<IChatSocket> Socket,
    const FChannelStateResponseFieldsDto& Dto)
{
    UChatChannel* Channel = NewObject<UChatChannel>(Outer);
    check(Socket->IsConnected());

    Channel->Api = Api;
    Channel->Socket = Socket;
    Channel->Properties = FChannelProperties{Dto, UUserManager::Get()};
    Channel->State = FChannelState{Dto, UUserManager::Get()};

    Channel->On<FMessageNewEvent>(Channel, &UChatChannel::OnMessageNew);
    Channel->On<FNotificationMessageNewEvent>(Channel, &UChatChannel::OnNotificationMessageNew);

    Channel->On<FMessageUpdatedEvent>(Channel, &UChatChannel::OnMessageUpdated);
    Channel->On<FMessageDeletedEvent>(Channel, &UChatChannel::OnMessageDeleted);
    Channel->On<FReactionNewEvent>(Channel, &UChatChannel::OnReactionNew);
    Channel->On<FReactionUpdatedEvent>(Channel, &UChatChannel::OnReactionUpdated);
    Channel->On<FReactionDeletedEvent>(Channel, &UChatChannel::OnReactionDeleted);
    Channel->On<FTypingStartEvent>(Channel, &UChatChannel::OnTypingStart);
    Channel->On<FTypingStopEvent>(Channel, &UChatChannel::OnTypingStop);

    if (Channel->Properties.Config.bReadEvents)
    {
        Channel->On<FMessageReadEvent>(Channel, &UChatChannel::OnMessageRead);
        Channel->On<FNotificationMarkReadEvent>(Channel, &UChatChannel::OnNotificationMessageRead);
    }

    Channel->MessagesUpdated.Broadcast(Channel->State.GetMessages());

    return Channel;
}

void UChatChannel::Delete(TFunction<void()> Callback, const bool bHardDelete) const
{
    Api->DeleteChannel(
        [Callback](const FDeleteChannelResponseDto&)
        {
            if (Callback)
            {
                Callback();
            }
        },
        Properties.Type,
        Properties.Id);
}

void UChatChannel::SendMessage(const FMessage& Message)
{
    // TODO Wait for attachments to upload

    FMessage NewMessage = Message;
    NewMessage.State = EMessageSendState::Sending;
    if (NewMessage.Id.IsEmpty())
    {
        NewMessage.Id = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
    }
    if (!NewMessage.User.IsValid())
    {
        NewMessage.User = UUserManager::Get()->GetCurrentUser();
    }
    if (NewMessage.CreatedAt.GetTicks() == 0)
    {
        NewMessage.CreatedAt = FDateTime::UtcNow();
    }
    if (NewMessage.UpdatedAt.GetTicks() == 0)
    {
        NewMessage.UpdatedAt = FDateTime::UtcNow();
    }

    const FMessageRequestDto Request = NewMessage.ToRequestDto(Properties.Cid);
    AddMessage(NewMessage);
    Api->SendNewMessage(
        Properties.Type,
        Properties.Id,
        Request,
        false,
        [](const FMessageResponseDto& Response)
        {
            // No need to add message here as the backend will send a websocket message
            UE_LOG(LogTemp, Log, TEXT("Sent message [Id=%s]"), *Response.Message.Id);
        });
    MessageSent.Broadcast(NewMessage);

    // TODO Cooldown?
    // TODO Retry logic
}

void UChatChannel::UpdateMessage(const FMessage& Message)
{
    // TODO Attachments

    FMessage UpdatedMessage = Message;
    UpdatedMessage.State = EMessageSendState::Updating;
    AddMessage(UpdatedMessage);

    const FMessageRequestDto Request = UpdatedMessage.ToRequestDto(Properties.Cid);
    Api->UpdateMessage(
        Request,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this)](const FMessageResponseDto& Response)
        {
            if (WeakThis.IsValid())
            {
                WeakThis->AddMessage(FMessage{Response.Message, UUserManager::Get()});
                UE_LOG(LogTemp, Log, TEXT("Updated message [Id=%s]"), *Response.Message.Id);
            }
        });
    // TODO retry?
}

void UChatChannel::DeleteMessage(const FMessage& Message)
{
    // TODO Attachments

    // Directly deleting the local messages which are not yet sent to server
    if (Message.State == EMessageSendState::Sending || Message.State == EMessageSendState::Failed)
    {
        FMessage DeletedMessage = Message;
        DeletedMessage.Type = EMessageType::Deleted;
        DeletedMessage.State = EMessageSendState::Sent;
        AddMessage(DeletedMessage);
        return;
    }

    FMessage DeletedMessage = Message;
    DeletedMessage.Type = EMessageType::Deleted;
    DeletedMessage.State = EMessageSendState::Deleting;
    if (!DeletedMessage.DeletedAt)
    {
        DeletedMessage.DeletedAt.Emplace(FDateTime::UtcNow());
    }
    AddMessage(DeletedMessage);

    Api->DeleteMessage(
        DeletedMessage.Id,
        false,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this)](const FMessageResponseDto& Response)
        {
            if (WeakThis.IsValid())
            {
                WeakThis->AddMessage(FMessage{Response.Message, UUserManager::Get()});
                UE_LOG(LogTemp, Log, TEXT("Deleted message [Id=%s]"), *Response.Message.Id);
            }
        });

    // TODO retry?
}

void UChatChannel::QueryAdditionalMessages(const EPaginationDirection Direction, const int32 Limit, const TFunction<void()> Callback)
{
    const bool bChannelEmpty = State.GetMessages().Num() == 0;
    if (bChannelEmpty)
    {
        return;
    }

    const FMessagePaginationOptions MessagePagination = [&]
    {
        FMessagePaginationOptions Options;
        Options.Limit = Limit;
        if (Direction == EPaginationDirection::Top)
        {
            Options.IdLt = State.GetMessages()[0].Id;
        }
        else
        {
            Options.IdGte = State.GetMessages().Last().Id;
        }
        return Options;
    }();

    Query(Callback, EChannelFlags::State, MessagePagination);
}

void UChatChannel::MarkRead(const TOptional<FMessage>& Message)
{
    const TOptional<FString> MessageId = Message.IsSet() ? Message.GetValue().Id : TOptional<FString>{};
    MarkRead(MessageId);
}

void UChatChannel::MarkRead(const FMessage& Message)
{
    const TOptional<FString> MessageId = Message.Id.IsEmpty() ? TOptional<FString>{} : Message.Id;
    MarkRead(MessageId);
}

void UChatChannel::MarkRead(const TOptional<FString>& MessageId)
{
    if (Properties.Config.bReadEvents && State.UnreadCount() > 0)
    {
        Api->MarkChannelRead({}, Properties.Type, Properties.Id, MessageId);

        // Clear unread count straight away locally
        State.MarkRead();
        UnreadChanged.Broadcast(0);
    }
}

const TArray<FMessage>& UChatChannel::GetMessages() const
{
    return State.GetMessages();
}

void UChatChannel::Query(
    TFunction<void()> Callback,
    const EChannelFlags Flags,
    const TOptional<FMessagePaginationOptions> MessagePagination,
    const TOptional<FUserPaginationOptions> MemberPagination,
    const TOptional<FUserPaginationOptions> WatcherPagination)
{
    Api->QueryChannel(
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Callback](const FChannelStateResponseDto& Dto)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            WeakThis->MergeState(Dto);

            if (Callback)
            {
                Callback();
            }
        },
        Properties.Type,
        Socket->GetConnectionId(),
        Flags,
        {},
        Properties.Id,
        Util::Convert<FMessagePaginationParamsRequestDto>(MessagePagination),
        Util::Convert<FPaginationParamsRequestDto>(MemberPagination),
        Util::Convert<FPaginationParamsRequestDto>(WatcherPagination));
}

void UChatChannel::SearchMessages(
    TFunction<void(const TArray<FMessage>&)> Callback,
    const TOptional<FString>& Query,
    const TOptional<FFilter>& MessageFilter,
    const TArray<FMessageSortOption>& Sort,
    const TOptional<uint32> MessageLimit) const
{
    TOptional<TSharedRef<FJsonObject>> MessageFilterJson;
    if (MessageFilter.IsSet())
    {
        MessageFilterJson.Emplace(MessageFilter.GetValue().ToJsonObject());
    }

    Api->SearchMessages(
        [Callback](const FSearchResponseDto& Response)
        {
            // Don't add the messages to this channel's state, just return
            if (Callback)
            {
                Callback(FMessage::FromSearchResults(Response.Results));
            }
        },
        FFilter::Equal(TEXT("cid"), Properties.Cid).ToJsonObject(),
        Query,
        MessageFilterJson,
        Util::Convert<FSortParamRequestDto>(Sort),
        MessageLimit);
}

void UChatChannel::SearchMessages(
    const FString& Query,
    const FFilter& MessageFilter,
    const TArray<FMessageSortOption>& Sort,
    const int32 MessageLimit,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    TArray<FMessage>& OutMessages)
{
    const TOptional<FString> OptionalQuery = Query.IsEmpty() ? TOptional<FString>{} : Query;
    const TOptional<FFilter> OptionalMessageFilter = MessageFilter.IsValid() ? MessageFilter : TOptional<FFilter>{};
    const TOptional<uint32> OptionalMessageLimit = MessageLimit > 0 ? static_cast<uint32>(MessageLimit) : TOptional<uint32>{};
    TCallbackAction<TArray<FMessage>>::CreateLatentAction(
        WorldContextObject,
        LatentInfo,
        OutMessages,
        [&](auto Callback) { SearchMessages(Callback, OptionalQuery, OptionalMessageFilter, Sort, OptionalMessageLimit); });
}

void UChatChannel::SendReaction(const FMessage& Message, const FName& ReactionType, const bool bEnforceUnique)
{
    FMessage NewMessage{Message};
    // Remove all previous reactions current user did
    if (bEnforceUnique)
    {
        NewMessage.Reactions.RemoveReactionWhere([](const FReaction& R) { return R.User.IsCurrent(); });
    }

    const FReaction NewReaction{ReactionType, UUserManager::Get()->GetCurrentUser(), Message.Id};
    NewMessage.Reactions.AddReaction(NewReaction);

    NewMessage.Reactions.UpdateOwnReactions();

    AddMessage(NewMessage);

    Api->SendReaction(Message.Id, {Message.Id, 1, ReactionType}, bEnforceUnique, false);
}

void UChatChannel::DeleteReaction(const FMessage& Message, const FReaction& Reaction)
{
    FMessage NewMessage{Message};
    NewMessage.Reactions.RemoveReactionWhere([&Reaction](const FReaction& R)
                                             { return R.User == Reaction.User && R.Type == Reaction.Type && R.MessageId == Reaction.MessageId; });

    NewMessage.Reactions.UpdateOwnReactions();

    AddMessage(NewMessage);
    Api->DeleteReaction(Message.Id, Reaction.Type);
}

void UChatChannel::KeyStroke(const FString& ParentMessageId)
{
    if (!Properties.Config.bTypingEvents)
    {
        return;
    }

    const FDateTime Now = FDateTime::UtcNow();
    if (!LastKeystrokeAt.IsSet() || (Now - LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
    {
        UE_LOG(LogTemp, Log, TEXT("Start typing"));
        const FUser& CurrentUser = *UUserManager::Get()->GetCurrentUser();
        SendEvent(FTypingStartEvent{
            Now,
            Properties.Id,
            Properties.Type,
            Properties.Cid,
            ParentMessageId,
            Util::Convert<FUserObjectDto>(CurrentUser),
        });
    }
    LastKeystrokeAt.Emplace(Now);

    GetWorld()->GetTimerManager().SetTimer(
        TypingTimerHandle,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), ParentMessageId]
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            if (WeakThis->LastKeystrokeAt.IsSet() && (FDateTime::UtcNow() - WeakThis->LastKeystrokeAt.GetValue()).GetTotalSeconds() >= WeakThis->TypingTimeout)
            {
                WeakThis->SendStopTypingEvent(ParentMessageId);
            }
        },
        TypingTimeout,
        false);
}

void UChatChannel::StopTyping(const FString& ParentMessageId)
{
    if (!Properties.Config.bTypingEvents)
    {
        return;
    }

    if (TypingTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
    }
    if (LastKeystrokeAt.IsSet() && (FDateTime::UtcNow() - LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
    {
        SendStopTypingEvent(ParentMessageId);
    }
}

void UChatChannel::SendStopTypingEvent(const FString& ParentMessageId)
{
    const FUser& CurrentUser = *UUserManager::Get()->GetCurrentUser();
    UE_LOG(LogTemp, Log, TEXT("Stop typing"));
    SendEvent(FTypingStopEvent{
        FDateTime::UtcNow(),
        Properties.Id,
        Properties.Type,
        Properties.Cid,
        ParentMessageId,
        Util::Convert<FUserObjectDto>(CurrentUser),
    });
}

void UChatChannel::OnTypingStart(const FTypingStartEvent& Event)
{
    const FUserRef User = UUserManager::Get()->UpsertUser(Event.User);
    OnTypingIndicator.Broadcast(ETypingIndicatorState::StartTyping, User);
}

void UChatChannel::OnTypingStop(const FTypingStopEvent& Event)
{
    const FUserRef User = UUserManager::Get()->UpsertUser(Event.User);
    OnTypingIndicator.Broadcast(ETypingIndicatorState::StopTyping, User);
}

void UChatChannel::MergeState(const FChannelStateResponseFieldsDto& Dto)
{
    check(!Properties.Cid.IsEmpty());
    UUserManager* UserManager = UUserManager::Get();
    State.Append(Dto, UserManager);
    MessagesUpdated.Broadcast(State.GetMessages());
}

void UChatChannel::AddMessage(const FMessage& Message)
{
    State.AddMessage(Message);

    MessagesUpdated.Broadcast(State.GetMessages());
}

FMessage MakeMessage(const FMessageEvent& Event)
{
    return FMessage{Event.Message, UUserManager::Get()};
}

void UChatChannel::OnMessageNew(const FMessageNewEvent& Event)
{
    HandleNewMessageEvent(Event);
}

void UChatChannel::OnNotificationMessageNew(const FNotificationMessageNewEvent& Event)
{
    HandleNewMessageEvent(Event);
}

void UChatChannel::HandleNewMessageEvent(const FMessageEvent& Event)
{
    const FMessage Message = MakeMessage(Event);
    AddMessage(Message);

    MessageReceived.Broadcast(Message);
    UnreadChanged.Broadcast(State.UnreadCount());
}

void UChatChannel::OnMessageUpdated(const FMessageUpdatedEvent& Event)
{
    AddMessage(MakeMessage(Event));
}

void UChatChannel::OnMessageDeleted(const FMessageDeletedEvent& Event)
{
    AddMessage(MakeMessage(Event));
}

void UChatChannel::OnMessageRead(const FMessageReadEvent& Event)
{
    const FUserRef EventUser = UUserManager::Get()->UpsertUser(Event.User);
    UpdateUnread(EventUser, 0, Event.CreatedAt);
}

void UChatChannel::OnNotificationMessageRead(const FNotificationMarkReadEvent& Event)
{
    const FUserRef EventUser = UUserManager::Get()->UpsertUser(Event.User);
    const int32 Unread = Event.TotalUnreadCount;
    UpdateUnread(EventUser, Unread, Event.CreatedAt);
}

void UChatChannel::UpdateUnread(const FUserRef& User, const int32 UnreadCount, const FDateTime& LastRead)
{
    if (FRead* Read = State.Read.FindByPredicate([&User](const FRead& R) { return R.User == User; }))
    {
        Read->UnreadMessages = UnreadCount;
        Read->LastRead = LastRead;
    }
    else
    {
        State.Read.Add(FRead{User, UnreadCount, LastRead});
    }
    if (User.IsCurrent())
    {
        UnreadChanged.Broadcast(UnreadCount);
    }
}

void UChatChannel::OnReactionNew(const FReactionNewEvent& Event)
{
    FMessage Message = MakeMessage(Event);
    Message.Reactions.UpdateOwnReactions();
    AddMessage(Message);
}

void UChatChannel::OnReactionUpdated(const FReactionUpdatedEvent& Event)
{
    FMessage Message = MakeMessage(Event);
    Message.Reactions.UpdateOwnReactions();
    AddMessage(Message);
}

void UChatChannel::OnReactionDeleted(const FReactionDeletedEvent& Event)
{
    FMessage Message = MakeMessage(Event);
    Message.Reactions.UpdateOwnReactions();
    AddMessage(Message);
}
