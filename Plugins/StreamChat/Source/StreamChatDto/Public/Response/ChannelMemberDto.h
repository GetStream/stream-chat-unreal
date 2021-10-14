// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserDto.h"

#include "ChannelMemberDto.generated.h"

/**
 * #/components/schemas/ChannelMember
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FChannelMemberDto
{
    GENERATED_BODY()

    /// Expiration date of the ban
    UPROPERTY()
    FDateTime BanExpires;

    /// Whether member is banned in this channel or not
    UPROPERTY()
    bool bBanned;

    /// Role of the member in the channel
    FString ChannelRole;

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// Date when invite was accepted
    UPROPERTY()
    FDateTime InviteAcceptedAt;

    /// Date when invite was rejected
    UPROPERTY()
    FDateTime InviteRejectedAt;

    /// Whether member was invited or not
    UPROPERTY()
    bool bInvited;

    /// Whether member is channel moderator or not
    UPROPERTY()
    bool bIsModerator;

    /// Whether member is shadow banned in this channel or not
    UPROPERTY()
    bool bShadowBanned;

    /// Date/time of the last update
    UPROPERTY()
    FDateTime UpdatedAt;

    UPROPERTY()
    FUserDto User;

    UPROPERTY()
    FString UserId;
};
