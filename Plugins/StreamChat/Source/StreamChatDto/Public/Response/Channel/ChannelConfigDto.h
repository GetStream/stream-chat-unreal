// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Message/CommandDto.h"

#include "ChannelConfigDto.generated.h"

/**
 *
 */
USTRUCT()
struct FChannelConfigDto
{
    GENERATED_BODY()

    // Moderation configuration
    FString Automod = TEXT("flag");

    /// List of available commands
    TArray<FCommandDto> Commands;

    /// True if the channel should send connect events
    bool bConnectEvents;

    /// Date of channel creation
    FDateTime CreatedAt;

    /// Date of last channel update
    FDateTime UpdatedAt;

    /// Max channel message length
    int32 MaxMessageLength;

    /// Duration of message retention
    FString MessageRetention;

    /// True if users can be muted
    bool bMutes;

    /// True if reaction are active for this channel
    bool bReactions;

    /// True if readEvents are active for this channel
    bool bReadEvents;

    /// True if reply message are active for this channel
    bool bReplies;

    /// True if it's possible to perform a search in this channel
    bool bSearch;

    /// True if typing events should be sent for this channel
    bool bTypingEvents;

    /// True if it's possible to upload files to this channel
    bool bUploads;

    /// True if urls appears as attachments
    bool bURLEnrichment;
};
