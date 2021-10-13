// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MessageRequestDto.generated.h"

/**
 * @see https://getstream.io/chat/docs/rest/#messages-sendmessage-request
 */
USTRUCT()
struct FMessageRequestDto
{
    GENERATED_BODY()

    // Not in spec, but seems needed
    UPROPERTY()
    FString Id;

    UPROPERTY()
    FString Text;

    // TODO Everything else
};
