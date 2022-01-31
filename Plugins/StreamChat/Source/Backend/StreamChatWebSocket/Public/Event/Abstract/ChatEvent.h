// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ChatEvent.generated.h"

/**
 * @brief Represents an event that happened in Stream Chat
 * @see https://getstream.io/chat/docs/unreal/event_object/#event-object
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FChatEvent
{
    GENERATED_BODY()

    /// Event type
    UPROPERTY()
    FName Type;

    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};
};
