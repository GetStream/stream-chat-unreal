// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ChatChannel.h"

#include "Api/ChatApi.h"
#include "StreamJson.h"

UChatChannel* UChatChannel::Create(const TSharedRef<FChatApi>& InApi, const FString& Type, const FString& Id)
{
    UChatChannel* Channel = NewObject<UChatChannel>();
    Channel->Api = InApi;
    Channel->Type = Type;
    Channel->Id = Id;
    return Channel;
}

void UChatChannel::Watch(const TFunction<void(const FChannelState&)> Callback)
{
    Api->GetOrCreateChannel(Callback, Type, true, true);
}
