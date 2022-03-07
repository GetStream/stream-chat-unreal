// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ReactionGroup.h"

#include "Reactions.generated.h"

struct FUserRef;
struct FReaction;
struct FReactionDto;

/**
 * @brief Encapsulates a collection of reactions for a message
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FReactions
{
    GENERATED_BODY()

    /// Create a collection of reactions using the data in the format received from the server
    static FReactions CollectReactions(
        UUserManager* UserManager,
        const TMap<FName, int32>& ReactionCounts,
        const TMap<FName, int32>& ReactionScores,
        const TArray<FReactionDto>& LatestReactions,
        const TArray<FReactionDto>& OwnReactions);

    /// Add a new reaction to the collection
    void AddReaction(const FReaction&);

    /// Remove a reaction according to the provided predicate
    void RemoveReactionWhere(TFunctionRef<bool(const FReaction&)> Predicate);

    /// Are there any reactions here?
    bool IsEmpty() const;

    /// Get the reaction created by the current user, if it exists
    TOptional<FReaction> GetOwnReaction(const FName& ReactionType) const;

    /// Get all reactions of this message, grouped by their type
    const TMap<FName, FReactionGroup>& GetReactionGroups() const;

    /// Get the scores of each of the reaction types in this collection
    TMap<FName, int32> GetScores() const;

    /// Remove OwnReactions which are NOT the given user ID
    void UpdateOwnReactions();

private:
    /// All reactions of this message, grouped by their type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message", meta = (AllowPrivateAccess))
    TMap<FName, FReactionGroup> ReactionGroups;
};

/**
 * @brief Blueprint functions for the Reactions struct
 * @ingroup StreamChat
 */
UCLASS()
class STREAMCHAT_API UReactionsBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /// Are ones of these reactions created by the current user?
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Reactions", meta = (DisplayName = "Has Own Reaction"))
    static bool HasOwnReaction(const FReactions& Reactions, const FName& ReactionType);

    /// Are there any reactions here?
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Reactions")
    static bool IsEmpty(const FReactions& Reactions);
};
