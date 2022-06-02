// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Moderation/MutedChannel.h"
#include "Moderation/MutedUser.h"

#include "OwnUser.generated.h"

struct FOwnUserDto;

/**
 * @brief The currently connected Stream Chat user
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FOwnUser
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FOwnUser();

    explicit FOwnUser(const FUserRef&);
    /// Convert from OwnUser
    explicit FOwnUser(const FOwnUserDto&, UUserManager*);

    void Update(const FOwnUser& OwnUser);

    /// Has this user muted the given user?
    bool HasMutedUser(const FUserRef& TargetUser) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat|User")
    FUserRef User;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    int32 UnreadCount = 0;

    /// Only populated for current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    int32 TotalUnreadCount = 0;

    /// Only populated for current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    int32 UnreadChannels = 0;

    /// Only populated for current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    TArray<FMutedUser> MutedUsers;

    /// Only populated for current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    TArray<FMutedChannel> MutedChannels;
};
