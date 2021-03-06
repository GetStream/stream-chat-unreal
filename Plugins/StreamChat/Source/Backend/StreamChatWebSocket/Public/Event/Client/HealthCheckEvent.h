// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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

    static FName StaticType()
    {
        static const FName Type{TEXT("health.check")};
        return Type;
    }

    UPROPERTY()
    FString ConnectionId;

    UPROPERTY()
    FOwnUserDto Me;

    // TODO everything else
};
