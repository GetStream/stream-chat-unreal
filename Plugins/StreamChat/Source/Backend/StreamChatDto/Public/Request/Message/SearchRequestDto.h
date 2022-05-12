// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Request/SortParamRequestDto.h"

#include "SearchRequestDto.generated.h"

/**
 * @brief #/components/schemas/SearchRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/search/
 */
USTRUCT()
struct STREAMCHATDTO_API FSearchRequestDto
{
    GENERATED_BODY()

    /// Number of items to return
    UPROPERTY()
    uint32 Limit = TNumericLimits<uint32>::Max();

    /// Item offset
    UPROPERTY()
    uint32 Offset = TNumericLimits<uint32>::Max();

    UPROPERTY()
    FJsonObjectWrapper FilterConditions;

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
