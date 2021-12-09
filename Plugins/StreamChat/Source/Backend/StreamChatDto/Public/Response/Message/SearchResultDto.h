// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SearchResultMessageDto.h"

#include "SearchResultDto.generated.h"

/**
 * @brief #/components/schemas/SearchResult
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FSearchResultDto
{
    GENERATED_BODY()

    /// Found message
    UPROPERTY()
    FSearchResultMessageDto Message;
};
