// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CustomEvents.h"

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"

namespace
{
UChatChannel* Channel = nullptr;

// https://getstream.io/chat/docs/unreal/custom_events/?language=unreal#to-a-channel
void ToAChannel()
{
    // Send the custom event
    Channel->SendEvent(FMyCustomEvent{5000.f});
}
}    // namespace
