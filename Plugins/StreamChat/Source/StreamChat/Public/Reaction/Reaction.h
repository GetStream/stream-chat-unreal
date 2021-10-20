#pragma once

#include "CoreMinimal.h"
#include "User.h"

#include "Reaction.generated.h"

struct FReactionDto;

USTRUCT(BlueprintType)
struct FReaction
{
    GENERATED_BODY()

    FReaction() = default;
    explicit FReaction(const FReactionDto&);
    explicit FReaction(const FName& Type, const FUser&, const FString& MessageId);

    /// The type of reaction (e.g. 'like', 'laugh', 'wow')
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Reaction")
    FName Type;

    /// Reaction score. If not specified reaction has score of 1
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Reaction")
    int32 Score = 1;

    /// Reacting user
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Reaction")
    FUser User;

    /// ID of a message user reacted to
    FString MessageId;

    /// Date/time of creation
    FDateTime CreatedAt = FDateTime::Now();

    /// Date/time of the last update
    FDateTime UpdatedAt = FDateTime::Now();
};
