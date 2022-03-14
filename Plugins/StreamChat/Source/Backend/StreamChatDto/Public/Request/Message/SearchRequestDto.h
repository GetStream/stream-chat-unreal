// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "Request/PaginatedRequest.h"

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

    FSearchRequestDto() = default;
    explicit FSearchRequestDto(const TOptional<uint32>& Limit, const TOptional<uint32>& Offset, const FJsonObjectWrapper& Filter);

    /// Search phrase
    void SetQuery(const FString&);
    /// Message filter conditions
    void SetMessageFilter(const TSharedRef<FJsonObject>&);
    /// Sort parameters. Cannot be used with non-zero offset
    void SetSort(const TArray<FSortParamRequestDto>&);
    /// Pagination parameter. Cannot be used with non-zero offset
    void SetNext(const FString&);

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
