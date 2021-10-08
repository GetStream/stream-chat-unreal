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
    // TODO can this can be an enum
    UPROPERTY()
    FName Type;

    /// Channel ID
    UPROPERTY()
    FString Cid;

    UPROPERTY()
    FDateTime CreatedAt;

    // TODO Me?
};
