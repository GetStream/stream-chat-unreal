// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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
    FDateTime CreatedAt = FDateTime{0};

    /// Date of last channel update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    /// Channel type name
    UPROPERTY()
    FString Name;

    /// True if typing events should be sent for this channel
    UPROPERTY()
    bool bTypingEvents = false;

    /// True if readEvents are active for this channel
    UPROPERTY()
    bool bReadEvents = false;

    /// True if the channel should send connect events
    UPROPERTY()
    bool bConnectEvents = false;

    /// True if it's possible to perform a search in this channel
    UPROPERTY()
    bool bSearch = false;

    /// True if reaction are active for this channel
    UPROPERTY()
    bool bReactions = false;

    /// True if reply message are active for this channel
    UPROPERTY()
    bool bReplies = false;

    /// True if users can be muted
    UPROPERTY()
    bool bMutes = false;

    /// True if it's possible to upload files to this channel
    UPROPERTY()
    bool bUploads = false;

    /// True if urls appears as attachments
    UPROPERTY()
    bool bURLEnrichment = false;

    /// Enables custom events
    UPROPERTY()
    bool bCustomEvents = false;

    /// Enables push notifications
    UPROPERTY()
    bool bPushNotifications = false;

    /// Number of days to keep messages. 'infinite' disables retention
    UPROPERTY()
    FString MessageRetention;

    /// Number of maximum message characters
    /// Max: 20000
    UPROPERTY()
    int32 MaxMessageLength = -1;

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
