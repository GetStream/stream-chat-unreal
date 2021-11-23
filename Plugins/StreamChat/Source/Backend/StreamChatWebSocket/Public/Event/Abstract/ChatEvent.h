// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ChatEvent.generated.h"

/**
 * Represents an event that happened in Stream Chat
 * https://getstream.io/chat/docs/other-rest/event_object/#event-object
 */
USTRUCT()
struct FChatEvent
{
    GENERATED_BODY()

    /// Event type
    UPROPERTY()
    FName Type;

    UPROPERTY()
    FDateTime CreatedAt;
};
