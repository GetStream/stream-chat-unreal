// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"

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
    const TSharedRef<FUserManager> UserManager,
    const FChannelStateResponseFieldsDto& Dto)
{
    UChatChannel* Channel = NewObject<UChatChannel>(Outer);
    check(Socket->IsConnected());

    Channel->Api = Api;
    Channel->Socket = Socket;
    Channel->UserManager = UserManager;
    Channel->State = FChannelState{Dto, *UserManager};
    Channel->On<FMessageNewEvent>(Channel, &UChatChannel::OnMessageNew);
    Channel->On<FMessageUpdatedEvent>(Channel, &UChatChannel::OnMessageUpdated);
    Channel->On<FMessageDeletedEvent>(Channel, &UChatChannel::OnMessageDeleted);
    Channel->On<FReactionNewEvent>(Channel, &UChatChannel::OnReactionNew);
    Channel->On<FReactionUpdatedEvent>(Channel, &UChatChannel::OnReactionUpdated);
    Channel->On<FReactionDeletedEvent>(Channel, &UChatChannel::OnReactionDeleted);
    Channel->On<FTypingStartEvent>(Channel, &UChatChannel::OnTypingStart);
    Channel->On<FTypingStopEvent>(Channel, &UChatChannel::OnTypingStop);

    Channel->MessagesUpdated.Broadcast(Channel->State.Messages);

    return Channel;
}

void UChatChannel::SendMessage(const FString& Text)
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
    };
    const FMessage LocalMessage{Request, UserManager->GetCurrentUser()};
    AddMessage(LocalMessage);
    Api->SendNewMessage(
        State.Type,
        State.Id,
        Request,
        false,
        [this](const FMessageResponseDto& Response)
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
        [this](const FMessageResponseDto& Response)
        {
            AddMessage(FMessage{*UserManager, Response.Message});
            UE_LOG(LogTemp, Log, TEXT("Updated message [Id=%s]"), *Response.Message.Id);
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
        [this](const FMessageResponseDto& Response)
        {
            AddMessage(FMessage{*UserManager, Response.Message});
            UE_LOG(LogTemp, Log, TEXT("Deleted message [Id=%s]"), *Response.Message.Id);
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
    const bool bChannelEmpty = State.Messages.Num() == 0;
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
            Dto.IdLt = State.Messages[0].Id;
        }
        else
        {
            Dto.IdGte = State.Messages.Last().Id;
        }
        return Dto;
    }();

    Api->QueryChannel(
        [this, Direction, Limit](const FChannelStateResponseDto& Dto)
        {
            if (!IsValid(this))
            {
                return;
            }

            if (Dto.Messages.Num() == 0 || Dto.Messages.Num() < Limit)
            {
                // Don't need to paginate again in this direction in the future
                EndedPaginationDirections |= Direction;
            }

            MergeState(Dto);

            SetPaginationRequestState(EHttpRequestState::Ended, Direction);
        },
        State.Type,
        Socket->GetConnectionId(),
        EChannelFlags::State,
        {},
        State.Id,
        MessagePagination);
}

inline void UChatChannel::SetPaginationRequestState(
    const EHttpRequestState RequestState,
    const EPaginationDirection Direction)
{
    PaginationRequestState = RequestState;
    OnPaginatingMessages.Broadcast(Direction, RequestState);
}

const TArray<FMessage>& UChatChannel::GetMessages() const
{
    return State.Messages;
}

void UChatChannel::SendReaction(const FMessage& Message, const FName& ReactionType, const bool bEnforceUnique)
{
    FMessage NewMessage{Message};
    // Remove all previous reactions current user did
    if (bEnforceUnique)
    {
        NewMessage.Reactions.RemoveReactionWhere([this](const FReaction& R) { return R.User.IsCurrent(); });
    }

    const FReaction NewReaction{ReactionType, UserManager->GetCurrentUser(), Message.Id};
    NewMessage.Reactions.AddReaction(NewReaction);

    NewMessage.Reactions.UpdateOwnReactions();

    AddMessage(NewMessage);

    Api->SendReaction(Message.Id, {Message.Id, 1, ReactionType}, bEnforceUnique, false);
}

void UChatChannel::DeleteReaction(const FMessage& Message, const FReaction& Reaction)
{
    FMessage NewMessage{Message};
    NewMessage.Reactions.RemoveReactionWhere(
        [&Reaction](const FReaction& R)
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
        SendEvent(FTypingStartEvent{
            {{FTypingStartEvent::StaticType, Now}, State.Id, State.Type, State.Cid},
            ParentMessageId,
            Util::Convert<FUserObjectDto>(**UserManager->GetCurrentUser()),
        });
    }
    LastKeystrokeAt.Emplace(Now);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        [this, ParentMessageId, TypingTimeout]()
        {
            const FDateTime Now = FDateTime::UtcNow();
            if (!LastKeystrokeAt.IsSet() || (Now - LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
            {
                UE_LOG(LogTemp, Log, TEXT("Stop typing"));
                SendEvent(FTypingStopEvent{
                    {{FTypingStopEvent::StaticType, Now}, State.Id, State.Type, State.Cid},
                    ParentMessageId,
                    Util::Convert<FUserObjectDto>(**UserManager->GetCurrentUser()),
                });
            }
        },
        2.f,
        false);
}

void UChatChannel::OnTypingStart(const FTypingStartEvent& Event)
{
    const FUserRef User = UserManager->UpsertUser(Event.User);
    OnTypingIndicator.Broadcast(ETypingIndicatorState::StartTyping, User);
}

void UChatChannel::OnTypingStop(const FTypingStopEvent& Event)
{
    const FUserRef User = UserManager->UpsertUser(Event.User);
    OnTypingIndicator.Broadcast(ETypingIndicatorState::StopTyping, User);
}

void UChatChannel::MergeState(const FChannelStateResponseFieldsDto& Dto)
{
    check(!State.Cid.IsEmpty());
    State.Merge(Dto, *UserManager);
    MessagesUpdated.Broadcast(State.Messages);
}

void UChatChannel::AddMessage(const FMessage& Message)
{
    State.AddMessage(Message);

    MessagesUpdated.Broadcast(State.Messages);
}

void UChatChannel::OnMessageNew(const FMessageNewEvent& Event)
{
    AddMessage(FMessage{*UserManager, Event.Message});
}

void UChatChannel::OnMessageUpdated(const FMessageUpdatedEvent& Event)
{
    AddMessage(FMessage{*UserManager, Event.Message});
}

void UChatChannel::OnMessageDeleted(const FMessageDeletedEvent& Event)
{
    AddMessage(FMessage{*UserManager, Event.Message});
}

void UChatChannel::OnReactionNew(const FReactionNewEvent& Event)
{
    FMessage Message{*UserManager, Event.Message};
    Message.Reactions.UpdateOwnReactions();
    AddMessage(Message);
}

void UChatChannel::OnReactionUpdated(const FReactionUpdatedEvent& Event)
{
    FMessage Message{*UserManager, Event.Message};
    Message.Reactions.UpdateOwnReactions();
    AddMessage(Message);
}

void UChatChannel::OnReactionDeleted(const FReactionDeletedEvent& Event)
{
    FMessage Message{*UserManager, Event.Message};
    Message.Reactions.UpdateOwnReactions();
    AddMessage(Message);
}
