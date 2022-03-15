// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Response/Moderation/BanResponseDto.h"
#include "User/UserRef.h"

#include "Ban.generated.h"

/**
 * @brief A banned user
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FBan
{
    GENERATED_BODY()
    FBan() = default;
    explicit FBan(const FBanResponseDto&, UStreamChatClientComponent*, UUserManager*);

    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FUserRef BannedBy;

    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    UChatChannel* Channel;

    /// Date/time of creation
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FDateTime CreatedAt = FDateTime{0};

    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FDateTime Expires = FDateTime{0};

    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    bool bShadow;

    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FUserRef User;
};
