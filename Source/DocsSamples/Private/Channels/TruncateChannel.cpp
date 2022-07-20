// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"

namespace TruncateChannel
{

UChatChannel* Channel = nullptr;
FMessage Message = {};

// https://getstream.io/chat/docs/unreal/truncate_channel/?language=unreal
void TruncateChannel()
{
    Channel->Truncate();

    // Or with parameters:
    Channel->Truncate(
        true,    // bHardDelete
        {},      // TruncatedAt
        FMessage{TEXT("Dear Everyone. The channel has been truncated.")},
        false,    // bSkipPush
        []
        {
            // Channel is truncated
        });
}
}    // namespace TruncateChannel
