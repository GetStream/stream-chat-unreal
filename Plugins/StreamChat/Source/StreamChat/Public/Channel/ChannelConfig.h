#pragma once

#include "Command.h"
#include "CoreMinimal.h"

#include "ChannelConfig.generated.h"

struct FChannelConfigWithInfoDto;

USTRUCT(BlueprintType)
struct FChannelConfig
{
    GENERATED_BODY()

    FChannelConfig() = default;
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
