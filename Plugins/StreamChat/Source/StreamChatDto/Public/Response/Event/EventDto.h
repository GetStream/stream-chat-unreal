﻿// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Channel/ChannelMemberDto.h"

#include "EventDto.generated.h"

/**
 * An event returned in a HTTP response. Distinct from WebSocket events
 * #/components/schemas/Event
 */
USTRUCT()
struct FEventDto
{
    GENERATED_BODY()

    /// Event type. To use custom event types see Custom Events documentation
    UPROPERTY()
    FString Type;

    UPROPERTY()
    FString Cid;

    UPROPERTY()
    FChannelMemberDto Member;

    UPROPERTY()
    FString Team;

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt;

    // TODO other fields?
};