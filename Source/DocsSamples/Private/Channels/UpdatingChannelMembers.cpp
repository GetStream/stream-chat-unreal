// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"

namespace
{
UChatChannel* Channel = nullptr;

// https://getstream.io/chat/docs/unreal/channel_members/?language=unreal#adding-removing-channel-members
void AddingAndRemovingChannelMembers()
{
    Channel->AddMembers({TEXT("thierry"), TEXT("josh")});
    Channel->RemoveMembers({TEXT("tommaso")});
}

// https://getstream.io/chat/docs/unreal/channel_members/?language=unreal#message-parameter
void MessageParameter()
{
    Channel->AddMembers({TEXT("tommaso")}, FMessage{TEXT("Tommaso joined the channel.")});
}

// Hide History: server-side only

// https://getstream.io/chat/docs/unreal/channel_members/?language=unreal#leaving-a-channel
void LeavingAChannel()
{
    Channel->RemoveMembers({TEXT("my-user-id")});
}

// Adding & Removing Moderators to a Channel: server-side only

}    // namespace
