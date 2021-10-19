#pragma once

#include "CoreMinimal.h"
#include "Reaction.h"

#include "ReactionGroup.generated.h"

struct FReactionDto;

/**
 * An abstraction to group reactions by their type.
 */
USTRUCT(BlueprintType)
struct FReactionGroup
{
    GENERATED_BODY()

    static TMap<FName, FReactionGroup> CollectReactions(
        const TMap<FName, int32>& ReactionCounts,
        const TMap<FName, int32>& ReactionScores,
        const TArray<FReactionDto>& LatestReactions,
        const TArray<FReactionDto>& OwnReactions);

    static TMap<FName, int32> GetScores(const TMap<FName, FReactionGroup>&);

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
