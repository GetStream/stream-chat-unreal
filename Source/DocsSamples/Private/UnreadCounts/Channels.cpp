// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Event/Notification/NotificationMarkReadEvent.h"
#include "Event/Notification/NotificationMessageNewEvent.h"
#include "StreamChatClientComponent.h"

namespace Channels
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/unread/?language=unreal
void Channels()
{
    const FChannelProperties Properties{TEXT("channel-type"), TEXT("channel-id")};
    Client->WatchChannel(
        Properties,
        [](UChatChannel* Channel)
        {
            // ReadState is the list of read states for each user on the channel
            const TArray<FRead>& ReadState = Channel->State.Read;
        });
}

// https://getstream.io/chat/docs/unreal/unread_channel/?language=unreal#unread-messages-per-channel
void UnreadMessagesPerChannel()
{
    // Not yet supported in the Unreal SDK
}

// https://getstream.io/chat/docs/unreal/unread_channel/?language=unreal#mark-all-as-read
void MarkAllAsRead()
{
    // Not yet supported in the Unreal SDK
}

}    // namespace Channels
