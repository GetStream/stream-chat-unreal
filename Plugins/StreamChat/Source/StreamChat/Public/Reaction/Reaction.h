// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "User/UserRef.h"

#include "Reaction.generated.h"

struct FReactionDto;

/**
 * @brief An individual reaction to a message.
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FReaction
{
    GENERATED_BODY()

    FReaction();
    explicit FReaction(const FReactionDto&, UUserManager*);
    explicit FReaction(const FName& Type, const FUserRef&, const FString& MessageId, int32 Score = 1);

    /// The type of reaction (e.g. 'like', 'laugh', 'wow')
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Reaction")
    FName Type;

    /// Reaction score. If not specified reaction has score of 1
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Reaction")
    int32 Score = 1;

    /// Reacting user
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Reaction")
    FUserRef User;

    /// ID of a message user reacted to
    FString MessageId;

    /// Date/time of creation
    FDateTime CreatedAt = FDateTime::UtcNow();

    /// Date/time of the last update
    FDateTime UpdatedAt = FDateTime::UtcNow();
};
