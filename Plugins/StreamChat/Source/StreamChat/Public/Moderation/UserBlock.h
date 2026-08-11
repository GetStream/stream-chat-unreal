// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Moderation/GetBlockedUsersResponseDto.h"
#include "User/UserRef.h"

#include "UserBlock.generated.h"

class UUserManager;

/**
 * @brief One user blocking another
 *
 * Blocking hides every 1-on-1 channel the two share from the blocking user, and stops the blocked
 * user's events and push notifications reaching them. Group channels are unaffected.
 *
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUserBlock
{
    GENERATED_BODY()

    FUserBlock() = default;
    explicit FUserBlock(const FBlockedUserDto&, UUserManager*);

    /// The user who was blocked
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FUserRef BlockedUser;

    /// The user who created the block, which for anything a client can query is the current user
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FUserRef BlockedBy;

    /// When the block was created
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FDateTime CreatedAt = FDateTime{0};
};
