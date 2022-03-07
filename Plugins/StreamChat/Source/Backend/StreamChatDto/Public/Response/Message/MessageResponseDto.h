// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageDto.h"

#include "MessageResponseDto.generated.h"

/**
 * @brief #/components/schemas/MessageResponse
 * @ingroup StreamChatDto
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
