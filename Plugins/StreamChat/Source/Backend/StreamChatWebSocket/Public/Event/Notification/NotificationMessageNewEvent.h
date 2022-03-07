// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "NotificationMessageNewEvent.generated.h"

/**
 * @brief Notification when a new message was created
 * @see https://getstream.io/chat/docs/events/#message.new
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FNotificationMessageNewEvent : public FMessageEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("notification.message_new")};
        return Type;
    }

    // Not in spec, but mentioned in docs
    UPROPERTY()
    int32 TotalUnreadCount = -1;

    // Not in spec, but mentioned in docs
    UPROPERTY()
    int32 UnreadChannels = -1;
};
