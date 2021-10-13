// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageDto.h"

#include "MessageResponseDto.generated.h"

/**
 * #/components/schemas/MessageResponse
 */
USTRUCT()
struct FMessageResponseDto
{
    GENERATED_BODY()

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    UPROPERTY()
    FMessageDto Message;
};
