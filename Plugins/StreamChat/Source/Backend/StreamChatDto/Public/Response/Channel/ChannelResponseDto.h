// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "ChannelConfigWithInfoDto.h"
#include "ChannelMemberDto.h"
#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "ChannelResponseDto.generated.h"

/**
 * @brief #/components/schemas/ChannelResponse
 * Represents channel in chat
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FChannelResponseDto
{
    GENERATED_BODY()

    /// Whether auto translation is enabled or not
    UPROPERTY()
    bool bAutoTranslationEnabled = false;

    /// Language to translate to when auto translation is active
    UPROPERTY()
    FString AutoTranslationLanguage;

    /// The cid of this channel
    UPROPERTY()
    FString Cid;

    /// The channel configuration data
    UPROPERTY()
    FChannelConfigWithInfoDto Config;

    /// Cooldown period after sending each message
    UPROPERTY()
    int32 Cooldown = -1;

    /// The date of channel creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// The user that created this channel
    UPROPERTY()
    FUserObjectDto CreatedBy;

    /// The date of channel deletion
    UPROPERTY()
    FDateTime DeletedAt = FDateTime{0};

    UPROPERTY()
    bool bDisabled = false;

    /// Whether this channel is frozen or not
    UPROPERTY()
    bool bFrozen = false;

    /// Whether this channel is hidden by current user or not
    UPROPERTY()
    bool bHidden = false;

    /// Date since when the message history is accessible
    UPROPERTY()
    FDateTime HideMessagesBefore = FDateTime{0};

    /// The id of this channel
    UPROPERTY()
    FString Id;

    /// The date of the last message
    UPROPERTY()
    FDateTime LastMessageAt = FDateTime{0};

    /// The count of this channel members
    UPROPERTY()
    int32 MemberCount = 0;

    /// List of channel members (max 100)
    UPROPERTY()
    TArray<FChannelMemberDto> Members;

    /// Date of mute expiration
    UPROPERTY()
    FDateTime MuteExpiresAt = FDateTime{0};

    /// Whether this channel is muted or not
    UPROPERTY()
    bool bMuted = false;

    /// List of channel capabilities of the authenticated user
    UPROPERTY()
    TArray<FString> OwnCapabilities;

    /// The team the channel belongs to (multi-tenant only)
    UPROPERTY()
    FString Team;

    /// The type of this channel
    UPROPERTY()
    FString Type;

    /// The date of the last channel update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    /// The date of channel deletion
    UPROPERTY()
    FDateTime TruncatedAt = FDateTime{0};

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
