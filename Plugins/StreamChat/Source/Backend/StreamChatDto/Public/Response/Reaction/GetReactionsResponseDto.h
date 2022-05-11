// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReactionDto.h"
#include "Response/ResponseDto.h"

#include "GetReactionsResponseDto.generated.h"

/**
 * @brief #/components/schemas/GetReactionsResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FGetReactionsResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// List of reactions
    UPROPERTY()
    TArray<FReactionDto> Reactions;
};
