// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageRequestDto.h"

#include "UpdateMessageRequestDto.generated.h"

/**
 * #/components/schemas/UpdateMessageRequest
 */
USTRUCT()
struct FUpdateMessageRequestDto
{
    GENERATED_BODY()

    /**
     * Message object
     */
    UPROPERTY()
    FMessageRequestDto Message;
};
