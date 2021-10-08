// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"

#include "Api/ChatApi.h"
#include "Api/Dto/Request/MessageRequestDto.h"
#include "Api/Dto/Response/ChannelStateResponseDto.h"
#include "Api/Dto/Response/MessageResponseDto.h"

UChatChannel* UChatChannel::Create(
    const TSharedRef<FChatApi>& InApi,
    const FString& ConnectionId,
    const FString& Type,
    const FString& Id)
{
    check(!ConnectionId.IsEmpty());

    UChatChannel* Channel = NewObject<UChatChannel>();
    Channel->Api = InApi;
    Channel->ConnectionId = ConnectionId;
    Channel->Type = Type;
    Channel->Id = Id;
    return Channel;
}

void UChatChannel::Watch(const TFunction<void()> Callback)
{
    check(!ConnectionId.IsEmpty());

    Api->GetOrCreateChannel(
        Type,
        ConnectionId,
        Id,
        EChannelCreationFlags::State | EChannelCreationFlags::Watch,
        [this, Callback](const FChannelStateResponseDto& State)
        {
            Messages.Empty(State.Messages.Num());
            for (auto&& Message : State.Messages)
            {
                Messages.Add(FMessage{Message});
            }

            if (Callback)
            {
                Callback();
            }
        });
}

void UChatChannel::SendMessage(const FString& Message)
{
    Api->SendNewMessage(
        [](const FMessageResponseDto& Response)
        {
            UE_LOG(LogTemp, Log, TEXT("Sent message [Id=%s]"), *Response.Message.Id);
        },
        Type,
        Id,
        {Message});
}

const TArray<FMessage>& UChatChannel::GetMessages() const
{
    return Messages;
}
