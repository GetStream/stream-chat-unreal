// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Message/CommandDto.h"

#include "ChannelConfigWithInfoDto.generated.h"

/**
 * @brief #/components/schemas/ChannelConfigWithInfo
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FChannelConfigWithInfoDto
{
    GENERATED_BODY()

    /// Date of channel creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// Date of last channel update
    UPROPERTY()
    FDateTime UpdatedAt;

    /// Channel type name
    UPROPERTY()
    FString Name;

    /// True if typing events should be sent for this channel
    UPROPERTY()
    bool bTypingEvents;

    /// True if readEvents are active for this channel
    UPROPERTY()
    bool bReadEvents;

    /// True if the channel should send connect events
    UPROPERTY()
    bool bConnectEvents;

    /// True if it's possible to perform a search in this channel
    UPROPERTY()
    bool bSearch;

    /// True if reaction are active for this channel
    UPROPERTY()
    bool bReactions;

    /// True if reply message are active for this channel
    UPROPERTY()
    bool bReplies;

    /// True if users can be muted
    UPROPERTY()
    bool bMutes;

    /// True if it's possible to upload files to this channel
    UPROPERTY()
    bool bUploads;

    /// True if urls appears as attachments
    UPROPERTY()
    bool bURLEnrichment;

    /// Enables custom events
    UPROPERTY()
    bool bCustomEvents;

    /// Enables push notifications
    UPROPERTY()
    bool bPushNotifications;

    /// Number of days to keep messages. 'infinite' disables retention
    UPROPERTY()
    FString MessageRetention;

    /// Number of maximum message characters
    /// Max: 20000
    UPROPERTY()
    int32 MaxMessageLength;

    /// Enables automatic message moderation
    // TODO enum
    UPROPERTY()
    FString Automod;

    /// Sets behavior of automatic moderation
    // TODO enum
    UPROPERTY()
    FString AutomodBehavior;

    /// Name of the blocklist to use
    UPROPERTY()
    FString Blocklist;

    /// Sets behavior of blocklist
    // TODO enum
    UPROPERTY()
    FString BlocklistBehavior;

    // TODO Automod thresholds

    /// List of available commands
    UPROPERTY()
    TArray<FCommandDto> Commands;

    // TODO grants
};
