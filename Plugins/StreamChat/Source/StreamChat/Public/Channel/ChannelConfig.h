// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Command.h"
#include "CoreMinimal.h"

#include "ChannelConfig.generated.h"

struct FChannelConfigWithInfoDto;

/**
 * @brief The configuration, permissions and features of a channel, as defined by the "channel type"
 * @see https://getstream.io/chat/docs/other-rest/channel_features/?q=commands#channel-types-fields
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
    bool bConnectEvents;

    /// Date of channel creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// Date of last channel update
    UPROPERTY()
    FDateTime UpdatedAt;

    /// Max channel message length
    UPROPERTY()
    int32 MaxMessageLength;

    /// Duration of message retention
    UPROPERTY()
    FString MessageRetention;

    /// True if users can be muted
    UPROPERTY()
    bool bMutes;

    /// True if reaction are active for this channel
    UPROPERTY()
    bool bReactions;

    /// True if readEvents are active for this channel
    UPROPERTY()
    bool bReadEvents;

    /// True if reply message are active for this channel
    UPROPERTY()
    bool bReplies;

    /// True if it's possible to perform a search in this channel
    UPROPERTY()
    bool bSearch;

    /// True if typing events should be sent for this channel
    bool bTypingEvents;

    /// True if it's possible to upload files to this channel
    UPROPERTY()
    bool bUploads;

    /// True if urls appears as attachments
    UPROPERTY()
    bool bUrlEnrichment;
};
