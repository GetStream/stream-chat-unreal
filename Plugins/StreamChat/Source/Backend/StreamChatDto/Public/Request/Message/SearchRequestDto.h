// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"

#include "SearchRequestDto.generated.h"

struct FSortParamRequestDto;

/**
 * @brief #/components/schemas/SearchRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/search/
 */
USTRUCT()
struct STREAMCHATDTO_API FSearchRequestDto
{
    GENERATED_BODY()

    /// Search phrase
    void SetQuery(const FString&);
    /// Message filter conditions
    void SetMessageFilter(const TSharedRef<FJsonObject>&);
    /// Sort parameters. Cannot be used with non-zero offset
    void SetSort(const TArray<FSortParamRequestDto>&);
    /// Number of messages to return
    void SetMessageLimit(int32);
    /// Pagination offset. Cannot be used with sort or next.
    void SetOffset(int32);
    /// Pagination parameter. Cannot be used with non-zero offset
    void SetNext(const FString&);

    // Channel filter conditions
    UPROPERTY()
    FJsonObjectWrapper FilterConditions;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
