// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChatEvent.h"
#include "OwnUserDto.h"

#include "NotificationMutesUpdatedEvent.generated.h"

/**
 * @brief Notification when a new message was created
 * @see https://getstream.io/chat/docs/events/#message.new
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FNotificationMutesUpdatedEvent : public FChatEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("notification.mutes_updated")};
        return Type;
    }

    UPROPERTY()
    FOwnUserDto Me;
};
