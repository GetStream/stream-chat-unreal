// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Request/PaginatedRequest.h"
#include "Request/SortParamRequestDto.h"

#include "SearchRequestDto.generated.h"

struct FSortParamRequestDto;

/**
 * @brief #/components/schemas/SearchRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/search/
 */
USTRUCT()
struct STREAMCHATDTO_API FSearchRequestDto : public FPaginatedRequest
{
    GENERATED_BODY()

    /// Search phrase
    UPROPERTY()
    FString Query;
    /// Message filter conditions
    UPROPERTY()
    FJsonObjectWrapper MessageFilterConditions;
    /// Sort parameters. Cannot be used with non-zero offset
    UPROPERTY()
    TArray<FSortParamRequestDto> Sort;
    /// Pagination parameter. Cannot be used with non-zero offset
    FString Next;
};
