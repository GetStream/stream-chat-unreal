// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MessagePaginationParamsRequestDto.generated.h"

class FJsonObject;

/**
 * @brief #/components/schemas/MessagePaginationParamsRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FMessagePaginationParamsRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    uint32 Limit = TNumericLimits<uint32>::Max();

    UPROPERTY()
    uint32 Offset = TNumericLimits<uint32>::Max();

    UPROPERTY()
    FDateTime CreatedAtAfter = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtAfterOrEqual = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtBefore = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtBeforeOrEqual = FDateTime{0};

    UPROPERTY()
    FString IdGt;

    UPROPERTY()
    FString IdGte;

    UPROPERTY()
    FString IdLt;

    UPROPERTY()
    FString IdLte;
};
