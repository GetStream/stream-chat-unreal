#pragma once

#include "ChatEvent.h"
#include "CoreMinimal.h"

#include "HealthCheckEvent.generated.h"

/**
 * Represents an event that happened in Stream Chat
 */
USTRUCT()
struct FHealthCheckEvent : public FChatEvent
{
    GENERATED_BODY()

    UPROPERTY()
    FString ConnectionId;

    // TODO everything else
};