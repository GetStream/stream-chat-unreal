// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"

#include "PaginatedRequest.generated.h"

/**
 * @brief #/components/schemas/SortParamRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FPaginatedRequest
{
    GENERATED_BODY()

    FPaginatedRequest() = default;
    explicit FPaginatedRequest(const TOptional<uint32>& Limit, const TOptional<uint32>& Offset, const FJsonObjectWrapper& Filter);

    /// Number of items to return
    void SetLimit(uint32);
    /// Pagination offset
    void SetOffset(uint32);

    UPROPERTY()
    FJsonObjectWrapper FilterConditions;

    UPROPERTY()
    FAdditionalFields Pagination;
};
