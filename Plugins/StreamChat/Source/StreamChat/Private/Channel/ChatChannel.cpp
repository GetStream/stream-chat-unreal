// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"

#include "Algo/Transform.h"
#include "Blueprint/CallbackAction.h"
#include "Channel/Filter.h"
#include "ChatApi.h"
#include "Engine/World.h"
#include "Event/Channel/MessageDeletedEvent.h"
#include "Event/Channel/MessageNewEvent.h"
#include "Event/Channel/MessageUpdatedEvent.h"
#include "Event/Channel/ReactionDeletedEvent.h"
#include "Event/Channel/ReactionNewEvent.h"
#include "Event/Channel/ReactionUpdatedEvent.h"
#include "Event/Channel/TypingStartEvent.h"
#include "Event/Channel/TypingStopEvent.h"
#include "Reaction/Reaction.h"
#include "Request/Message/MessageRequestDto.h"
#include "Request/Reaction/ReactionRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Message/MessageResponseDto.h"
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
    Channel->Properties = FChannelProperties{Dto, *UUserManager::Get()};
    Channel->State = FChannelState{Dto, *UUserManager::Get()};
    Channel->On<FMessageNewEvent>(Channel, &UChatChannel::OnMessageNew);
    Channel->On<FMessageUpdatedEvent>(Channel, &UChatChannel::OnMessageUpdated);
    Channel->On<FMessageDeletedEvent>(Channel, &UChatChannel::OnMessageDeleted);
    Channel->On<FReactionNewEvent>(Channel, &UChatChannel::OnReactionNew);
    Channel->On<FReactionUpdatedEvent>(Channel, &UChatChannel::OnReactionUpdated);
    Channel->On<FReactionDeletedEvent>(Channel, &UChatChannel::OnReactionDeleted);
    Channel->On<FTypingStartEvent>(Channel, &UChatChannel::OnTypingStart);
    Channel->On<FTypingStopEvent>(Channel, &UChatChannel::OnTypingStop);

    Channel->MessagesUpdated.Broadcast(Channel->State.GetMessages());

    return Channel;
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
                WeakThis->AddMessage(FMessage{*UUserManager::Get(), Response.Message});
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
                WeakThis->AddMessage(FMessage{*UUserManager::Get(), Response.Message});
                UE_LOG(LogTemp, Log, TEXT("Deleted message [Id=%s]"), *Response.Message.Id);
            }
        });

    // TODO retry?
}

void UChatChannel::QueryAdditionalMessages(const EPaginationDirection Direction, int32 Limit)
{
    const bool bPaginationEnded = EnumHasAnyFlags(EndedPaginationDirections, Direction);
    if (bPaginationEnded)
    {
        return;
    }
    if (PaginationRequestState == EHttpRequestState::Started)
    {
        return;
    }
    const bool bChannelEmpty = State.GetMessages().Num() == 0;
    if (bChannelEmpty)
    {
        return;
    }

    SetPaginationRequestState(EHttpRequestState::Started, Direction);

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

    const int32 OrigMessageCount = State.GetMessages().Num();
    Query(
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Direction, Limit, OrigMessageCount]
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            const int32 DeltaMessageCount = WeakThis->State.GetMessages().Num() - OrigMessageCount;
            if (DeltaMessageCount == 0 || DeltaMessageCount < Limit)
            {
                // Don't need to paginate again in this direction in the future
                WeakThis->EndedPaginationDirections |= Direction;
            }
            WeakThis->SetPaginationRequestState(EHttpRequestState::Ended, Direction);
        },
        EChannelFlags::State,
        MessagePagination);
}

inline void UChatChannel::SetPaginationRequestState(const EHttpRequestState RequestState, const EPaginationDirection Direction)
{
    PaginationRequestState = RequestState;
    OnPaginatingMessages.Broadcast(Direction, RequestState);
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

    constexpr float TypingTimeout = 2.f;
    const FDateTime Now = FDateTime::UtcNow();
    if (!LastKeystrokeAt.IsSet() || (Now - LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
    {
        UE_LOG(LogTemp, Log, TEXT("Start typing"));
        const FUser& CurrentUser = *UUserManager::Get()->GetCurrentUser();
        SendEvent(FTypingStartEvent{
            {{FTypingStartEvent::StaticType, Now}, Properties.Id, Properties.Type, Properties.Cid},
            ParentMessageId,
            Util::Convert<FUserObjectDto>(CurrentUser),
        });
    }
    LastKeystrokeAt.Emplace(Now);

    GetWorld()->GetTimerManager().SetTimer(
        TypingTimerHandle,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), ParentMessageId, TypingTimeout]
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            if (!WeakThis->LastKeystrokeAt.IsSet() || (FDateTime::UtcNow() - WeakThis->LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
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
    SendStopTypingEvent(ParentMessageId);
}

void UChatChannel::SendStopTypingEvent(const FString& ParentMessageId)
{
    const FUser& CurrentUser = *UUserManager::Get()->GetCurrentUser();
    UE_LOG(LogTemp, Log, TEXT("Stop typing"));
    SendEvent(FTypingStopEvent{
        {{FTypingStopEvent::StaticType, FDateTime::UtcNow()}, Properties.Id, Properties.Type, Properties.Cid},
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
    State.Append(Dto, *UserManager);
    MessagesUpdated.Broadcast(State.GetMessages());
}

void UChatChannel::AddMessage(const FMessage& Message)
{
    State.AddMessage(Message);

    MessagesUpdated.Broadcast(State.GetMessages());
}

FMessage MakeMessage(const FMessageEvent& Event)
{
    UUserManager* UserManager = UUserManager::Get();
    return FMessage{*UserManager, Event.Message};
}

void UChatChannel::OnMessageNew(const FMessageNewEvent& Event)
{
    const FMessage NewMessage = MakeMessage(Event);
    AddMessage(NewMessage);

    MessageReceived.Broadcast(NewMessage);
}

void UChatChannel::OnMessageUpdated(const FMessageUpdatedEvent& Event)
{
    AddMessage(MakeMessage(Event));
}

void UChatChannel::OnMessageDeleted(const FMessageDeletedEvent& Event)
{
    AddMessage(MakeMessage(Event));
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
