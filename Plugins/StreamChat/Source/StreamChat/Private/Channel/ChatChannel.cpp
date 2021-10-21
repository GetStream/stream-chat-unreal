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
#include "StreamChatClientComponent.h"
#include "TimerManager.h"
#include "Util.h"

UChatChannel* UChatChannel::Create(UStreamChatClientComponent& Client, const FString& Type, const FString& Id)
{
    UChatChannel* Channel = NewObject<UChatChannel>(&Client);
    Channel->Api = Client.GetApi();
    Channel->Socket = Client.GetSocket();
    Channel->Type = Type;
    Channel->Id = Id;
    Channel->User = Client.GetCurrentUser();
    Channel->On<FMessageNewEvent>(Channel, &UChatChannel::OnMessageNew);
    Channel->On<FMessageUpdatedEvent>(Channel, &UChatChannel::OnMessageUpdated);
    Channel->On<FMessageDeletedEvent>(Channel, &UChatChannel::OnMessageDeleted);
    Channel->On<FReactionNewEvent>(Channel, &UChatChannel::OnReactionNew);
    Channel->On<FReactionUpdatedEvent>(Channel, &UChatChannel::OnReactionUpdated);
    Channel->On<FReactionDeletedEvent>(Channel, &UChatChannel::OnReactionDeleted);

    check(!Channel->Socket->GetConnectionId().IsEmpty());

    return Channel;
}

UChatChannel* UChatChannel::Create(UStreamChatClientComponent& Client, const FChannelStateResponseFieldsDto& Fields)
{
    UChatChannel* Channel = Create(Client, Fields.Channel.Type, Fields.Channel.Id);
    Channel->InitializeState(Fields);

    return Channel;
}

void UChatChannel::Watch(const TFunction<void()> Callback)
{
    check(!Socket->GetConnectionId().IsEmpty());

    Api->GetOrCreateChannel(
        [this, Callback](const FChannelStateResponseDto& State)
        {
            if (!IsValid(this))
            {
                return;
            }

            InitializeState(State);

            if (Callback)
            {
                Callback();
            }
        },
        Type,
        Socket->GetConnectionId(),
        Id,
        EChannelFlags::State | EChannelFlags::Watch);
}

void UChatChannel::SendMessage(const FString& Text, const FUser& FromUser)
{
    // TODO Wait for attachments to upload

    const FMessageRequestDto Request{
        Cid,
        {},
        FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens),
        {},
        {},
        false,
        Text,
    };
    AddMessage(FMessage{Request, FromUser});
    Api->SendNewMessage(
        Type,
        Id,
        Request,
        false,
        [this](const FMessageResponseDto& Response)
        {
            AddMessage(Util::Convert<FMessage>(Response.Message));
            UE_LOG(LogTemp, Log, TEXT("Sent message [Id=%s]"), *Response.Message.Id);
        });

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
            AddMessage(Util::Convert<FMessage>(Response.Message));
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
        DeletedMessage.DeletedAt.Emplace(FDateTime::Now());
    }
    AddMessage(DeletedMessage);

    Api->DeleteMessage(
        DeletedMessage.Id,
        false,
        [this](const FMessageResponseDto& Response)
        {
            AddMessage(Util::Convert<FMessage>(Response.Message));
            UE_LOG(LogTemp, Log, TEXT("Deleted message [Id=%s]"), *Response.Message.Id);
        });

    // TODO retry?
}

const TArray<FMessage>& UChatChannel::GetMessages() const
{
    return Messages;
}

void UChatChannel::SendReaction(const FMessage& Message, const FName& ReactionType, const bool bEnforceUnique)
{
    FMessage NewMessage{Message};
    // Remove all previous reactions current user did
    if (bEnforceUnique)
    {
        NewMessage.Reactions.RemoveReactionWhere([this](const FReaction& R) { return R.User.Id == User.Id; });
    }

    const FReaction NewReaction{ReactionType, User, Message.Id};
    NewMessage.Reactions.AddReaction(NewReaction);

    NewMessage.Reactions.UpdateOwnReactions(User.Id);

    AddMessage(NewMessage);

    Api->SendReaction(Message.Id, {Message.Id, 1, ReactionType}, bEnforceUnique, false);
}

void UChatChannel::DeleteReaction(const FMessage& Message, const FReaction& Reaction)
{
    FMessage NewMessage{Message};
    NewMessage.Reactions.RemoveReactionWhere(
        [&Reaction](const FReaction& R)
        { return R.User.Id == Reaction.User.Id && R.Type == Reaction.Type && R.MessageId == Reaction.MessageId; });

    NewMessage.Reactions.UpdateOwnReactions(User.Id);

    AddMessage(NewMessage);

    Api->DeleteReaction(Message.Id, Reaction.Type);
}

void UChatChannel::KeyStroke(const FString& ParentMessageId)
{
    if (!Config.bTypingEvents)
    {
        return;
    }

    constexpr float TypingTimeout = 2.f;
    const FDateTime Now = FDateTime::Now();
    if (!LastKeystrokeAt.IsSet() || (Now - LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
    {
        UE_LOG(LogTemp, Log, TEXT("Start typing"));
        SendEvent(FTypingStartEvent{
            {{FTypingStartEvent::StaticType, Now}, Id, Type, Cid},
            ParentMessageId,
            Util::Convert<FUserObjectDto>(User),
        });
    }
    LastKeystrokeAt.Emplace(Now);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        [this, ParentMessageId, TypingTimeout]()
        {
            const FDateTime Now = FDateTime::Now();
            if (!LastKeystrokeAt.IsSet() || (Now - LastKeystrokeAt.GetValue()).GetTotalSeconds() >= TypingTimeout)
            {
                UE_LOG(LogTemp, Log, TEXT("Stop typing"));
                SendEvent(FTypingStopEvent{
                    {{FTypingStopEvent::StaticType, Now}, Id, Type, Cid},
                    ParentMessageId,
                    Util::Convert<FUserObjectDto>(User),
                });
            }
        },
        2.f,
        false);
}

const FString& UChatChannel::GetCid() const
{
    return Cid;
}

void UChatChannel::InitializeState(const FChannelStateResponseFieldsDto& State)
{
    Id = State.Channel.Id;
    Cid = State.Channel.Cid;
    Name = State.Channel.Name;
    ImageUrl = State.Channel.Image;
    Messages = Util::Convert<FMessage>(State.Messages);
    Config = Util::Convert<FChannelConfig>(State.Channel.Config);
    MessagesUpdated.Broadcast(Messages);
}

void UChatChannel::AddMessage(const FMessage& Message)
{
    // TODO Threads
    // TODO Quoting

    if (const int32 Index = Messages.FindLastByPredicate([&](const FMessage& M) { return M.Id == Message.Id; });
        Index != INDEX_NONE)
    {
        Messages[Index] = Message;
    }
    else
    {
        Messages.Add(Message);
    }

    MessagesUpdated.Broadcast(Messages);
}

void UChatChannel::OnMessageNew(const FMessageNewEvent& Event)
{
    AddMessage(Util::Convert<FMessage>(Event.Message));
}

void UChatChannel::OnMessageUpdated(const FMessageUpdatedEvent& Event)
{
    AddMessage(Util::Convert<FMessage>(Event.Message));
}

void UChatChannel::OnMessageDeleted(const FMessageDeletedEvent& Event)
{
    AddMessage(Util::Convert<FMessage>(Event.Message));
}

void UChatChannel::OnReactionNew(const FReactionNewEvent& Event)
{
    FMessage Message{Event.Message};
    Message.Reactions.UpdateOwnReactions(User.Id);
    AddMessage(Message);
}

void UChatChannel::OnReactionUpdated(const FReactionUpdatedEvent& Event)
{
    FMessage Message{Event.Message};
    Message.Reactions.UpdateOwnReactions(User.Id);
    AddMessage(Message);
}

void UChatChannel::OnReactionDeleted(const FReactionDeletedEvent& Event)
{
    FMessage Message{Event.Message};
    Message.Reactions.UpdateOwnReactions(User.Id);
    AddMessage(Message);
}
