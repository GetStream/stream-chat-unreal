// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Message.h"
#include "Read.h"

#include "ChannelState.generated.h"

struct FChannelStateResponseFieldsDto;

/**
 * @brief Encapsulates the local state of a channel
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FChannelState
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FChannelState();

    /// Create a channel state from a DTO from the API
    explicit FChannelState(const FChannelStateResponseFieldsDto&, UUserManager&);

    /// Merge new information from the API into this state
    void Append(const FChannelStateResponseFieldsDto&, UUserManager&);

    /// Locally add a message to this channel
    void AddMessage(const FMessage&);

    /// Get all the messages of this channel
    const TArray<FMessage>& GetMessages() const;

    /// Get the count of unread messages for the current user in this channel
    int32 UnreadCount() const;

    /// Number of channel watchers
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    int32 WatcherCount;

    /// The read states of each user in this channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    TArray<FRead> Read;

private:
    static TArray<FMessage> Convert(const FChannelStateResponseFieldsDto&, UUserManager&);

    TArray<FMessage> Messages;
};
