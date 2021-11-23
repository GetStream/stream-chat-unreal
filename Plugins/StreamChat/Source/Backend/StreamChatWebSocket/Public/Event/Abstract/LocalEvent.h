// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "LocalEvent.generated.h"

/**
 * Event not originating from WebSocket
 */
USTRUCT()
struct FLocalEvent
{
    GENERATED_BODY()

    /// Event type
    UPROPERTY()
    FName Type;
};
