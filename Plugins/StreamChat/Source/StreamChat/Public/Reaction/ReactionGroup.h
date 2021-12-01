// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Reaction.h"

#include "ReactionGroup.generated.h"

/**
 * @brief An abstraction to group reactions by their type.
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FReactionGroup
{
    GENERATED_BODY()

    /// The type of reaction (e.g. 'like', 'laugh', 'wow')
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Reaction")
    FName Type;

    /// Count of all reactions of this type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Reaction")
    int32 Count;

    /// Total reaction score of all reactions of this type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Reaction")
    int32 TotalScore;

    /// The latest reactions of this type created by any user.
    /// Note that `Count` might not equal `LatestReactions.Num()`, depending on how much data we have locally
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    TArray<FReaction> LatestReactions;

    /// The reaction of this type added to the message by the current user.
    TOptional<FReaction> OwnReaction;
};

/**
 * @brief Blueprint functions for the ReactionGroup struct
 * @ingroup StreamChat
 */
UCLASS()
class STREAMCHAT_API UReactionGroupBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /// Does this reaction group contain a reaction created by the current user?
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Reaction")
    static bool HasOwnReaction(const FReactionGroup& ReactionGroup);

    /// Get the reaction created by the current user, if it exists
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Reaction")
    static void GetOwnReaction(const FReactionGroup& ReactionGroup, bool& bIsSet, FReaction& Reaction);
};
