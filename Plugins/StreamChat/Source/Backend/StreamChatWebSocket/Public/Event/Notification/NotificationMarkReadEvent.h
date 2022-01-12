// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "NotificationMarkReadEvent.generated.h"

/**
 * @brief Notification when a message or entire channel was marked as read
 * @see https://getstream.io/chat/docs/events/#notification.mark_read

 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FNotificationMarkReadEvent : public FChannelEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("notification.mark_read");

    UPROPERTY()
    int32 TotalUnreadCount = -1;

    UPROPERTY()
    int32 UnreadChannels = -1;

    UPROPERTY()
    int32 UnreadCount = -1;

    UPROPERTY()
    FUserObjectDto User;
};
