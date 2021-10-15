// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"

#include "ChatApi.h"
#include "ChatSocketEvents.h"
#include "Detail/EventSubscription.h"
#include "Event/NewMessageEvent.h"
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
    Socket.Events().Subscribe<FNewMessageEvent>(
        TEventReceivedDelegate<FNewMessageEvent>::CreateUObject(Channel, &UChatChannel::OnNewMessage));

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

void UChatChannel::SendMessage(const FString& Message, const FUser& FromUser)
{
    // TODO Wait for attachments to upload

    const FMessageRequestDto Request{
        FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens),
        Message,
    };
    AddMessage(FMessage{Request, FromUser});
    Api->SendNewMessage(
        Type,
        Id,
        Request,
        false,
        [this](const FMessageResponseDto& Response)
        {
            AddMessage(FMessage{Response.Message});
            UE_LOG(LogTemp, Log, TEXT("Sent message [Id=%s]"), *Response.Message.Id);
        });

    // TODO Cooldown?
    // TODO Retry logic
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

void UChatChannel::OnNewMessage(const FNewMessageEvent& NewMessageEvent)
{
    AddMessage(FMessage{NewMessageEvent.Message});
}
