#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ReactionGroup.h"

#include "Reactions.generated.h"

struct FReaction;
struct FReactionDto;

USTRUCT(BlueprintType)
struct STREAMCHAT_API FReactions
{
    GENERATED_BODY()

    static FReactions CollectReactions(
        const TMap<FName, int32>& ReactionCounts,
        const TMap<FName, int32>& ReactionScores,
        const TArray<FReactionDto>& LatestReactions,
        const TArray<FReactionDto>& OwnReactions);

    TMap<FName, int32> GetScores() const;

    void AddReaction(const FReaction&);
    void RemoveReactionWhere(TFunctionRef<bool(const FReaction&)> Predicate);

    bool IsEmpty() const;
    bool HasOwnReaction(const FName& ReactionType) const;

    /// Remove OwnReactions which are NOT the given user ID
    void UpdateOwnReactions(const FString& OwnUserId);

    /// All reactions of this message, grouped by their type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    TMap<FName, FReactionGroup> ReactionGroups;
};

UCLASS()
class STREAMCHAT_API UReactionsBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Reactions", meta = (DisplayName = HasOwnReaction))
    static bool HasOwnReaction_Reactions(const FReactions& Reactions, const FName& ReactionType);

    UFUNCTION(BlueprintPure, Category = "Stream Chat|Reactions", meta = (DisplayName = HasOwnReaction))
    static bool HasOwnReaction_ReactionGroup(const FReactionGroup& ReactionGroup);

    UFUNCTION(BlueprintPure, Category = "Stream Chat|Reactions")
    static bool IsEmpty(const FReactions& Reactions);
};
