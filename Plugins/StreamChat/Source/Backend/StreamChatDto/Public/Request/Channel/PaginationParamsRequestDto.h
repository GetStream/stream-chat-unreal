// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PaginationParamsRequestDto.generated.h"

/**
 * @brief #/components/schemas/PaginationParamsRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FPaginationParamsRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    uint32 Limit = TNumericLimits<uint32>::Max();

    UPROPERTY()
    uint32 Offset = TNumericLimits<uint32>::Max();

    UPROPERTY()
    FString IdGt;

    UPROPERTY()
    FString IdGte;

    UPROPERTY()
    FString IdLt;

    UPROPERTY()
    FString IdLte;
};
