﻿// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelConfigDto.h"
#include "CoreMinimal.h"
#include "UserDto.h"

#include "ChannelModelDto.generated.h"

/**
 *
 */
USTRUCT()
struct FChannelModelDto
{
    GENERATED_BODY()

    /// The id of this channel
    UPROPERTY()
    FString Id;

    /// The type of this channel
    UPROPERTY()
    FString Type;

    /// The cid of this channel
    UPROPERTY()
    FString Cid;

    /// The channel configuration data
    UPROPERTY()
    FChannelConfigDto Config;

    /// The user that created this channel
    // TODO: Optional
    UPROPERTY()
    FUserDto CreatedBy;

    /// True if this channel is frozen
    UPROPERTY()
    bool bFrozen;

    /// The date of the last message
    // TODO: Optional
    UPROPERTY()
    FDateTime LastMessageAt;

    /// The date of channel creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// The date of the last channel update
    UPROPERTY()
    FDateTime UpdatedAt;

    /// The date of channel deletion
    // TODO: Optional
    UPROPERTY()
    FDateTime DeletedAt;

    /// The count of this channel members
    UPROPERTY()
    int32 MemberCount;

    /// The number of seconds in a cooldown
    UPROPERTY()
    int32 Cooldown;

    /// The team the channel belongs to
    // TODO: Optional
    UPROPERTY()
    FString Team;

    // TODO: extra data
};