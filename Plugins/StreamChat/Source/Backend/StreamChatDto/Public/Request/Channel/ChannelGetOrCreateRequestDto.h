﻿// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelRequestDto.h"
#include "CoreMinimal.h"
#include "MessagePaginationParamsRequestDto.h"
#include "PaginationParamsRequestDto.h"

#include "ChannelGetOrCreateRequestDto.generated.h"

class FJsonObject;

/**
 * #/components/schemas/ChannelGetOrCreateRequest
 */
USTRUCT()
struct FChannelGetOrCreateRequestDto
{
    GENERATED_BODY()

    STREAMCHATDTO_API static void SerializeExtra(const FChannelGetOrCreateRequestDto&, FJsonObject&);

    UPROPERTY()
    FString ConnectionId;

    UPROPERTY()
    FChannelRequestDto Data;

    /// Start watching the channel
    UPROPERTY()
    bool bWatch;

    /// Refresh channel state
    UPROPERTY()
    bool bState;

    /// Fetch user presence info
    UPROPERTY()
    bool bPresence;

    TOptional<FPaginationParamsRequestDto> Members;

    TOptional<FMessagePaginationParamsRequestDto> Messages;

    TOptional<FPaginationParamsRequestDto> Watchers;
};