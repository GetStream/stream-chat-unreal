// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelConfig.h"
#include "CoreMinimal.h"
#include "Member.h"
#include "Message.h"

#include "ChannelId.generated.h"

struct FChannelResponseDto;

/**
 * @brief Encapsulates the static properties of a channel
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FChannelId
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FChannelId();

    /// Create a channel ID from a DTO from the API
    explicit FChannelId(const FChannelResponseDto&, UUserManager&);

    /// Get user information of channel members other than the currently logged-in user
    TArray<FUserRef> GetOtherMemberUsers() const;

    /// Type of channel, either built-in or custom
    /// @see https://getstream.io/chat/docs/other-rest/channel_features/
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Type;

    /// An identifier for this channel. Not globally unique. A subset of the Cid.
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Id;

    /// The cid of this channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Cid;

    /// Configuration of the channel (usually set via the dashboard)
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FChannelConfig Config;

    /// The count of the members of this channel
    UPROPERTY()
    int32 MemberCount;

    /// The members of this channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    TArray<FMember> Members;

    /// Cooldown period after sending each message
    UPROPERTY()
    int32 Cooldown;

    /// The date of channel creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// The date of the last channel update
    UPROPERTY()
    FDateTime UpdatedAt;

    /// The date of channel deletion
    UPROPERTY()
    FDateTime DeletedAt;

    /// The date of the last message
    UPROPERTY()
    FDateTime LastMessageAt;

    /// The user that created this channel
    UPROPERTY()
    FUserRef CreatedBy;

    UPROPERTY()
    bool bDisabled;

    /// Whether this channel is frozen or not
    UPROPERTY()
    bool bFrozen;

    /// Whether this channel is hidden by current user or not
    UPROPERTY()
    bool bHidden;

    /// Whether this channel is muted or not
    UPROPERTY()
    bool bMuted;

    /// Date of mute expiration
    UPROPERTY()
    FDateTime MuteExpiresAt;

    /// Date since when the message history is accessible
    UPROPERTY()
    FDateTime HideMessagesBefore;

    /// List of channel capabilities of the authenticated user
    UPROPERTY()
    TArray<FString> OwnCapabilities;

    /// Whether auto translation is enabled or not
    UPROPERTY()
    bool bAutoTranslationEnabled;

    /// Language to translate to when auto translation is active
    UPROPERTY()
    FString AutoTranslationLanguage;

    /// The team the channel belongs to (multi-tenant only)
    UPROPERTY()
    FString Team;

    /// The human-readable name of this channel
    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Name;

    /// The URL of an image to show as the "profile-picture" of this channel
    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString ImageUrl;

private:
    void SetMembers(UUserManager&, const TArray<FChannelMemberDto>&);
};
