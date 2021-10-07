// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"

#include "Api/ChatApi.h"

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
        [this, Callback](const FChannelStateDto& State)
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
        },
        Type,
        ConnectionId,
        Id,
        true,
        true);
}
