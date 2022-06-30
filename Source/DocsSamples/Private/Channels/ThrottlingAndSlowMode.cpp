// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "TimerManager.h"

namespace ThrottlingAndSlowMode
{

UChatChannel* Channel = nullptr;
FMessage Message = {};
FTimerHandle TimerHandle;
UWorld* GetWorld()
{
    return nullptr;
}
void DisableSendMessageUI()
{
}
void EnableSendMessageUI()
{
}

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
    Channel->SendMessage(
        Message,
        [](const bool& bSuccess)
        {
            if (Channel->Properties.Cooldown > 0)
            {
                // first lock the UI so that the user is aware of the cooldown
                DisableSendMessageUI();
                // restore the UI after the cooldown is finished
                GetWorld()->GetTimerManager().SetTimer(
                    TimerHandle, [] { EnableSendMessageUI(); }, Channel->Properties.Cooldown.GetTotalSeconds(), false);
            }
        });
}
}    // namespace ThrottlingAndSlowMode
