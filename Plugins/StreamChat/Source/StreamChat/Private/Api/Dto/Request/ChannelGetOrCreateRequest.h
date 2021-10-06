// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ChannelGetOrCreateRequest.generated.h"

/**
 *
 */
USTRUCT()
struct FChannelGetOrCreateRequest
{
    GENERATED_BODY()

    // TODO uncomment
    // UPROPERTY()
    // FChannelRequest Data;

    /// Start watching the channel
    UPROPERTY()
    bool bWatch;

    /// Refresh channel state
    UPROPERTY()
    bool bState;

    /// Fetch user presence info
    UPROPERTY()
    bool bPresence;

    // TODO Messages, members, watchers
};
