﻿// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ChannelGetOrCreateRequestDto.generated.h"

/**
 *
 */
USTRUCT()
struct FChannelGetOrCreateRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    FString ConnectionId;

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