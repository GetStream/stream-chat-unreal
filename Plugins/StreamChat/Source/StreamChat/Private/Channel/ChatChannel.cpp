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

void UChatChannel::SendMessage(const FString& Text)
{
    SendMessageWithAdditionalFields(Text, {});
}

void UChatChannel::SendMessageWithAdditionalFields(const FString& Text, const FAdditionalFields& AdditionalFields)
{
    // TODO Wait for attachments to upload

    const FMessageRequestDto Request{
        State.Cid,
        {},
        FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens),
        {},
        {},
        false,
        Text,
        AdditionalFields,
    };
    const FMessage LocalMessage{Request, UUserManager::Get()->GetCurrentUser()};
    AddMessage(LocalMessage);
    Api->SendNewMessage(
        State.Type,
        State.Id,
        Request,
        false,
        [](const FMessageResponseDto& Response)
        {
            // No need to add message here as the backend will send a websocket message
            UE_LOG(LogTemp, Log, TEXT("Sent message [Id=%s]"), *Response.Message.Id);
        });
    MessageSent.Broadcast(LocalMessage);

    // TODO Cooldown?
    // TODO Retry logic
}

void UChatChannel::UpdateMessage(const FMessage& Message)
{
    // TODO Attachments

    FMessage UpdatedMessage = Message;
    UpdatedMessage.State = EMessageSendState::Updating;
    AddMessage(UpdatedMessage);

    Api->UpdateMessage(
        Util::Convert<FMessageRequestDto>(UpdatedMessage),
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

    const FMessagePaginationParamsRequestDto MessagePagination = [&]
    {
        FMessagePaginationParamsRequestDto Dto;
        Dto.Limit = Limit;
        if (Direction == EPaginationDirection::Top)
        {
            Dto.SetIdLt(State.GetMessages()[0].Id);
        }
        else
        {
            Dto.SetIdGte(State.GetMessages().Last().Id);
        }
        return Dto;
    }();

    Api->QueryChannel(
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), Direction, Limit](const FChannelStateResponseDto& Dto)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            if (Dto.Messages.Num() == 0 || Dto.Messages.Num() < Limit)
            {
                // Don't need to paginate again in this direction in the future
                WeakThis->EndedPaginationDirections |= Direction;
            }

            WeakThis->MergeState(Dto);

            WeakThis->SetPaginationRequestState(EHttpRequestState::Ended, Direction);
        },
        State.Type,
        Socket->GetConnectionId(),
        EChannelFlags::State,
        {},
        State.Id,
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
            UUserManager* UserManager = UUserManager::Get();
            TArray<FMessage> Messages;
            Algo::Transform(Response.Results, Messages, [UserManager](const FSearchResultDto& R) { return FMessage{*UserManager, R.Message}; });

            // Don't add the messages to this channel's state, just return
            if (Callback)
            {
                Callback(Messages);
            }
        },
        FFilter::Equal(TEXT("cid"), State.Cid).ToJsonObject(),
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
    if (!State.Config.bTypingEvents)
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
            {{FTypingStartEvent::StaticType, Now}, State.Id, State.Type, State.Cid},
            ParentMessageId,
            Util::Convert<FUserObjectDto>(CurrentUser),
        });
    }
    LastKeystrokeAt.Emplace(Now);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        [WeakThis = TWeakObjectPtr<UChatChannel>(this), ParentMessageId, TypingTimeout]()
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            const FDateTime Now = FDateTime::UtcNow();
            if (!WeakThis->LastKeystrokeAt.IsSet() || (Now - WeakThis->LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
            {
                const FUser& CurrentUser = *UUserManager::Get()->GetCurrentUser();
                UE_LOG(LogTemp, Log, TEXT("Stop typing"));
                WeakThis->SendEvent(FTypingStopEvent{
                    {{FTypingStopEvent::StaticType, Now}, WeakThis->State.Id, WeakThis->State.Type, WeakThis->State.Cid},
                    ParentMessageId,
                    Util::Convert<FUserObjectDto>(CurrentUser),
                });
            }
        },
        2.f,
        false);
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
    check(!State.Cid.IsEmpty());
    UUserManager* UserManager = UUserManager::Get();
    State.Merge(Dto, *UserManager);
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
