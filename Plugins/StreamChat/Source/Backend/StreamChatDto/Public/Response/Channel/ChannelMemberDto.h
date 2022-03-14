// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "ChannelMemberDto.generated.h"

/**
 * @brief #/components/schemas/ChannelMember
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FChannelMemberDto
{
    GENERATED_BODY()

    /// Expiration date of the ban
    UPROPERTY()
    FDateTime BanExpires = FDateTime{0};

    /// Whether member is banned in this channel or not
    UPROPERTY()
    bool bBanned = false;

    /// Role of the member in the channel
    UPROPERTY()
    FString ChannelRole;

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// Date when invite was accepted
    UPROPERTY()
    FDateTime InviteAcceptedAt = FDateTime{0};

    /// Date when invite was rejected
    UPROPERTY()
    FDateTime InviteRejectedAt = FDateTime{0};

    /// Whether member was invited or not
    UPROPERTY()
    bool bInvited = false;

    /// Whether member is channel moderator or not
    UPROPERTY()
    bool bIsModerator = false;

    /// Whether member is shadow banned in this channel or not
    UPROPERTY()
    bool bShadowBanned = false;

    /// Date/time of the last update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    UPROPERTY()
    FUserObjectDto User;

    UPROPERTY()
    FString UserId;
};
