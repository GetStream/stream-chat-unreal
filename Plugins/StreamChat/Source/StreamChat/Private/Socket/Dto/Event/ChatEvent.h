#pragma once

#include "CoreMinimal.h"

#include "ChatEvent.generated.h"

/**
 * Represents an event that happened in Stream Chat
 */
USTRUCT()
struct FChatEvent
{
    GENERATED_BODY()

    // TODO this can be an enum
    UPROPERTY()
    FString Type;
};
