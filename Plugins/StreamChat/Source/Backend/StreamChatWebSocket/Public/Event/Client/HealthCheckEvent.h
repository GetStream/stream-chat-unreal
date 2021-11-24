// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChatEvent.h"
#include "OwnUserDto.h"

#include "HealthCheckEvent.generated.h"

/**
 * @brief Sent periodically from the API to indicate a healthy connection
 * @see https://getstream.io/chat/docs/events/#health.check
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FHealthCheckEvent : public FChatEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("health.check");

    UPROPERTY()
    FString ConnectionId;

    UPROPERTY()
    FOwnUserDto Me;

    // TODO everything else
};
