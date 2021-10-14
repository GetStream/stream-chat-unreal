// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelConfigDto.h"
#include "ChannelMemberDto.h"
#include "CoreMinimal.h"
#include "UserDto.h"

#include "ChannelResponseDto.generated.h"

/**
 * Represents channel in chat
 * #/components/schemas/ChannelResponse
 */
USTRUCT()
struct FChannelResponseDto
{
    GENERATED_BODY()

    /// Whether auto translation is enabled or not
    UPROPERTY()
    bool bAutoTranslationEnabled;

    /// Language to translate to when auto translation is active
    UPROPERTY()
    FString AutoTranslationLanguage;

    /// The cid of this channel
    UPROPERTY()
    FString Cid;

    /// The channel configuration data
    UPROPERTY()
    FChannelConfigDto Config;

    /// Cooldown period after sending each message
    UPROPERTY()
    int32 Cooldown;

    /// The date of channel creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// The user that created this channel
    // TODO: Optional
    UPROPERTY()
    FUserDto CreatedBy;

    /// The date of channel deletion
    // TODO: Optional
    UPROPERTY()
    FDateTime DeletedAt;

    UPROPERTY()
    bool bDisabled;

    /// Whether this channel is frozen or not
    UPROPERTY()
    bool bFrozen;

    /// Whether this channel is hidden by current user or not
    UPROPERTY()
    bool bHidden;

    /// Date since when the message history is accessible
    UPROPERTY()
    FDateTime HideMessagesBefore;

    /// The id of this channel
    UPROPERTY()
    FString Id;

    /// The date of the last message
    // TODO: Optional
    UPROPERTY()
    FDateTime LastMessageAt;

    /// The count of this channel members
    UPROPERTY()
    int32 MemberCount;

    /// List of channel members (max 100)
    UPROPERTY()
    TArray<FChannelMemberDto> Members;

    /// Date of mute expiration
    UPROPERTY()
    FDateTime MuteExpiresAt;

    /// Whether this channel is muted or not
    UPROPERTY()
    bool bMuted;

    /// List of channel capabilities of the authenticated user
    UPROPERTY()
    TArray<FString> OwnCapabilities;

    /// The team the channel belongs to (multi-tenant only)
    // TODO: Optional
    UPROPERTY()
    FString Team;

    /// The type of this channel
    UPROPERTY()
    FString Type;

    /// The date of the last channel update
    UPROPERTY()
    FDateTime UpdatedAt;
};
