// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"
namespace
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/creating_channels/?language=unreal#1.-creating-a-channel-using-a-channel-id
void CreatingAChannelUsingAChannelId()
{
    const FChannelProperties Properties{
        TEXT("messaging"),    // Type
        TEXT("travel"),       // Id
    };
    Client->CreateChannel(
        Properties,
        [](UChatChannel* Channel)
        {
            // Channel created
        });
}

void CreatingAChannelForAListOfMembers()
{
    const FChannelProperties Properties = FChannelProperties{TEXT("messaging")}.SetMembers({TEXT("thierry"), TEXT("tommaso")});
    Client->CreateChannel(
        Properties,
        [](UChatChannel* Channel)
        {
            // Channel created
        });
}
}    // namespace
