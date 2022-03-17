// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Moderation/UserMuteDto.h"
#include "UserRef.h"

#include "MutedUser.generated.h"

USTRUCT(BlueprintType)
struct STREAMCHAT_API FMutedUser
{
    GENERATED_BODY()

public:
    FMutedUser();
    explicit FMutedUser(const FUserMuteDto&, UUserManager*);

    /// Date/time of creation
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted User")
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted User")
    FDateTime UpdatedAt = FDateTime{0};

    /// Date/time of mute expiration
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted User")
    FDateTime Expires = FDateTime{0};

    /// User who's muted
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted User")
    FUserRef Target;

    /// Owner of channel mute
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Muted User")
    FUserRef User;
};
