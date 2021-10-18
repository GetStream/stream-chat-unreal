// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"

#include "ChatApi.h"
#include "ChatSocketEvents.h"
#include "Event/MessageDeletedEvent.h"
#include "Event/MessageNewEvent.h"
#include "Event/MessageUpdatedEvent.h"
#include "IChatSocket.h"
#include "Request/MessageRequestDto.h"
#include "Response/ChannelStateResponseDto.h"
#include "Response/MessageResponseDto.h"
#include "Util.h"

UChatChannel*
UChatChannel::Create(const TSharedRef<FChatApi>& InApi, IChatSocket& Socket, const FString& Type, const FString& Id)
{
    UChatChannel* Channel = NewObject<UChatChannel>();
    Channel->Api = InApi;
    Channel->ConnectionId = Socket.GetConnectionId();
    Channel->Type = Type;
    Channel->Id = Id;
    Socket.Events().SubscribeUObject<FMessageNewEvent>(Channel, &UChatChannel::OnMessageNew);
    Socket.Events().SubscribeUObject<FMessageUpdatedEvent>(Channel, &UChatChannel::OnMessageUpdated);
    Socket.Events().SubscribeUObject<FMessageDeletedEvent>(Channel, &UChatChannel::OnMessageDeleted);

    check(!Channel->ConnectionId.IsEmpty());

    return Channel;
}

UChatChannel* UChatChannel::Create(
    const TSharedRef<FChatApi>& InApi,
    IChatSocket& Socket,
    const FChannelStateResponseFieldsDto& Fields)
{
    UChatChannel* Channel = Create(InApi, Socket, Fields.Channel.Type, Fields.Channel.Id);
    Channel->InitializeState(Fields);

    return Channel;
}

void UChatChannel::Watch(const TFunction<void()> Callback)
{
    check(!ConnectionId.IsEmpty());

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
        ConnectionId,
        Id,
        EChannelFlags::State | EChannelFlags::Watch);
}

void UChatChannel::SendMessage(const FString& Text, const FUser& FromUser)
{
    // TODO Wait for attachments to upload

    const FMessageRequestDto Request{
        FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens),
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

void UChatChannel::InitializeState(const FChannelStateResponseFieldsDto& State)
{
    Name = State.Channel.Name;
    ImageUrl = State.Channel.Image;
    Messages = Util::Convert<FMessage>(State.Messages);
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
