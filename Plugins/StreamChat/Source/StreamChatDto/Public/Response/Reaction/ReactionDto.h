// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserDto.h"

#include "ReactionDto.generated.h"

/**
 * Represents user reaction to a message
 * #/components/schemas/Reaction
 */
USTRUCT()
struct FReactionDto
{
    GENERATED_BODY()

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// ID of a message user reacted to
    UPROPERTY()
    FString MessageId;

    /// Reaction score. If not specified reaction has score of 1
    UPROPERTY()
    int32 Score = 1;

    /// The type of reaction (e.g. 'like', 'laugh', 'wow')
    UPROPERTY()
    FName Type;

    /// Date/time of the last update
    UPROPERTY()
    FDateTime UpdatedAt;

    UPROPERTY()
    FUserDto User;

    UPROPERTY()
    FString UserId;
};
