// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EventDto.h"

#include "EventResponseDto.generated.h"

/**
 * @brief #/components/schemas/EventResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FEventResponseDto
{
    GENERATED_BODY()

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    UPROPERTY()
    FEventDto Event;
};
