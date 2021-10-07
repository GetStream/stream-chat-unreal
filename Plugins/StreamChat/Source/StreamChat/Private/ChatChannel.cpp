// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ChatChannel.h"

#include "Api/ChatApi.h"

UChatChannel* UChatChannel::Create(
    const TSharedRef<FChatApi>& InApi,
    const FString& ConnectionId,
    const FString& Type,
    const FString& Id)
{
    UChatChannel* Channel = NewObject<UChatChannel>();
    Channel->Api = InApi;
    Channel->ConnectionId = ConnectionId;
    Channel->Type = Type;
    Channel->Id = Id;
    return Channel;
}

void UChatChannel::Watch(const TFunction<void(const FChannelState&)> Callback) const
{
    // TODO don't use bool flags
    Api->GetOrCreateChannel(Callback, Type, ConnectionId, Id, true, true);
}
