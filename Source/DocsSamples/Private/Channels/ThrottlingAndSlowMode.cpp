// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"

namespace ThrottlingAndSlowMode
{

UChatChannel* Channel = nullptr;

// https://getstream.io/chat/docs/unreal/slow_mode/?language=unreal#channel-slow-mode
void ChannelSlowMode()
{
    // Enable slow mode and set cooldown to 1s
    Channel->EnableSlowMode(FTimespan::FromSeconds(1.));

    // Increase cooldown to 30s
    Channel->EnableSlowMode(FTimespan::FromSeconds(30.));

    // Disable slow mode
    Channel->DisableSlowMode();
}

void UiCooldown()
{
    // Channel->SendMessage()
}
}    // namespace ThrottlingAndSlowMode
