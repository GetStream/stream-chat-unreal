// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Command.h"
#include "CoreMinimal.h"

#include "ChannelConfig.generated.h"

struct FChannelConfigWithInfoDto;

/**
 * @brief The configuration, permissions and features of a channel, as defined by the "channel type"
 * @see https://getstream.io/chat/docs/unreal/channel_features/?q=commands#channel-types-fields
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FChannelConfig
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FChannelConfig();

    /// Create a channel config from a DTO from the API
    explicit FChannelConfig(const FChannelConfigWithInfoDto&);

    /// Moderation configuration
    UPROPERTY()
    FString Automod;

    /// List of available commands
    UPROPERTY()
    TArray<FCommand> Commands;

    /// True if the channel should send connect events
    UPROPERTY()
    bool bConnectEvents = false;

    /// Date of channel creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// Date of last channel update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    /// Max channel message length
    UPROPERTY()
    int32 MaxMessageLength = 0;

    /// Duration of message retention
    UPROPERTY()
    FString MessageRetention;

    /// True if users can be muted
    UPROPERTY()
    bool bMutes = false;

    /// True if reaction are active for this channel
    UPROPERTY()
    bool bReactions = false;

    /// True if readEvents are active for this channel
    UPROPERTY()
    bool bReadEvents = false;

    /// True if reply message are active for this channel
    UPROPERTY()
    bool bReplies = false;

    /// True if it's possible to perform a search in this channel
    UPROPERTY()
    bool bSearch = false;

    /// True if typing events should be sent for this channel
    bool bTypingEvents = false;

    /// True if it's possible to upload files to this channel
    UPROPERTY()
    bool bUploads = false;

    /// True if urls appears as attachments
    UPROPERTY()
    bool bUrlEnrichment = false;
};
