// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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

    /// Identifies this WebSocket connection. Required on every API call made while it is open.
    UPROPERTY()
    FString ConnectionId;

    /// The connected user. Only sent on the first health check of a connection, which is what
    /// makes that one the signal that the connection is established.
    UPROPERTY()
    FOwnUserDto Me;

    /// Channel CID (<type>:<id>) for a watched channel's health check, or `*` for the
    /// connection-wide one, which is what the periodic keep-alive reply carries.
    UPROPERTY()
    FString Cid;
};
