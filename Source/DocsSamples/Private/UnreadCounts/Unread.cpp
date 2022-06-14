// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Event/Notification/NotificationMarkReadEvent.h"
#include "Event/Notification/NotificationMessageNewEvent.h"
#include "StreamChatClientComponent.h"

namespace Unread
{
UStreamChatClientComponent* Client = nullptr;
UChatChannel* Channel = nullptr;
FUser User;

// https://getstream.io/chat/docs/unreal/unread/?language=unreal
void Unread()
{
    Client->ConnectUser(
        User,
        TEXT("{{ chat_user_token }}"),
        [](const FOwnUser& UserRef)
        {
            const int32 TotalUnreadCount = UserRef.TotalUnreadCount;
            const int32 UnreadChannels = UserRef.UnreadChannels;
        });
}

void MarkRead()
{
    Channel->MarkRead();
}

void ReadEvents()
{
    Client->On<FNotificationMessageNewEvent>(
        [](const FNotificationMessageNewEvent& Event)
        {
            const int32 TotalUnreadCount = Event.TotalUnreadCount;
            const int32 UnreadChannels = Event.UnreadChannels;
        });

    Client->On<FNotificationMarkReadEvent>(
        [](const FNotificationMarkReadEvent& Event)
        {
            const int32 TotalUnreadCount = Event.TotalUnreadCount;
            const int32 UnreadChannels = Event.UnreadChannels;
        });
}

}    // namespace Unread
