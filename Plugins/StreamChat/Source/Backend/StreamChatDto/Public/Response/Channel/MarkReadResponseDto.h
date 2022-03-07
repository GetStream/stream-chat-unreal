// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Event/EventDto.h"

#include "MarkReadResponseDto.generated.h"

/**
 * @brief #/components/schemas/MarkReadResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMarkReadResponseDto
{
    GENERATED_BODY()

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    /// Mark read event
    UPROPERTY()
    FEventDto Event;
};
