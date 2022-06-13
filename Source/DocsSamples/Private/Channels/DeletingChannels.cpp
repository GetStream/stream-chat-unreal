// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"

namespace
{
UChatChannel* Channel = nullptr;

// https://getstream.io/chat/docs/unreal/channel_delete/?language=unreal#deleting-a-channel
void DeletingAChannel()
{
    Channel->Delete();
}
}    // namespace
