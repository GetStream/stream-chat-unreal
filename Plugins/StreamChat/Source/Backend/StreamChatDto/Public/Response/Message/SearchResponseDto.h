// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/ResponseDto.h"
#include "SearchResultDto.h"
#include "SearchWarningDto.h"

#include "SearchResponseDto.generated.h"

/**
 * @brief #/components/schemas/SearchResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FSearchResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// Value to pass to the next search query in order to paginate
    UPROPERTY()
    FString Next;

    /// Value that points to the previous page. Pass as the next value in a search query to paginate backwards
    UPROPERTY()
    FString Previous;

    /// Search results
    UPROPERTY()
    TArray<FSearchResultDto> Results;

    /// Warning about the search results
    UPROPERTY()
    FSearchWarningDto ResultsWarning;
};
