// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
    int32 Score = 1;

    /// The type of reaction (e.g. 'like', 'laugh', 'wow')
    UPROPERTY()
    FName Type;

    // Skipping user fields and they are never needed for client SDKs
};
