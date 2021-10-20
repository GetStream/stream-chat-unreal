﻿#pragma once

#include "CoreMinimal.h"
#include "Reaction.h"

#include "ReactionGroup.generated.h"

/**
 * An abstraction to group reactions by their type.
 */
USTRUCT(BlueprintType)
struct FReactionGroup
{
    GENERATED_BODY()

    bool HasOwnReaction() const;

    /// The type of reaction (e.g. 'like', 'laugh', 'wow')
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Reaction")
    FName Type;

    /// Count of all reactions of this type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Reaction")
    int32 Count;

    /// Total reaction score of all reactions of this type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Reaction")
    int32 TotalScore;

    /// The latest reactions of this type created by any user.
    /// Note that `Count` might not equal `LatestReactions.Num()`, depending on how much data we have locally
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    TArray<FReaction> LatestReactions;

    /// The reactions of this type added to the message by the current user.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    TArray<FReaction> OwnReactions;
};