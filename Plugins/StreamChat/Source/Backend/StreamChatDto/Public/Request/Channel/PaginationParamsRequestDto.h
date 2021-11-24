// Copyright Stream.IO, Inc. All Rights Reserved.

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
    int32 IdGt;

    UPROPERTY()
    int32 IdGte;

    UPROPERTY()
    int32 IdLt;

    UPROPERTY()
    int32 IdLte;

    UPROPERTY()
    uint32 Limit;

    UPROPERTY()
    int32 Offset;
};
