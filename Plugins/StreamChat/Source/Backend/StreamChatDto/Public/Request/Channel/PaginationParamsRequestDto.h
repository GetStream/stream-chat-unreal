// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
    int32 IdGt = -1;

    UPROPERTY()
    int32 IdGte = -1;

    UPROPERTY()
    int32 IdLt = -1;

    UPROPERTY()
    int32 IdLte = -1;

    UPROPERTY()
    uint32 Limit = -1;

    UPROPERTY()
    int32 Offset = -1;
};
