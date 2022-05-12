// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ReactionRequestDto.generated.h"

/**
 * @brief #/components/schemas/ReactionRequest
 * Represents user reaction to a message
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FReactionRequestDto
{
    GENERATED_BODY()

    /// ID of a message react reacted to
    UPROPERTY()
    FString MessageId;

    /// Reaction score. If not specified reaction has score of 1
    UPROPERTY()
    uint32 Score = TNumericLimits<uint32>::Max();

    /// The type of reaction (e.g. 'like', 'laugh', 'wow')
    UPROPERTY()
    FName Type;
};
