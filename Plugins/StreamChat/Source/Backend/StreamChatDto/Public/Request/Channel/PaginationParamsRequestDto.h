// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
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

    void SetIdGt(const FString&);

    void SetIdGte(const FString&);

    void SetIdLt(const FString&);

    void SetIdLte(const FString&);

    UPROPERTY()
    uint32 Limit = 20;

    UPROPERTY()
    int32 Offset = 0;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
