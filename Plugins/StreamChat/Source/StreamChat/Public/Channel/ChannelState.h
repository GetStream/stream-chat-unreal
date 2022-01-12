// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelConfig.h"
#include "CoreMinimal.h"
#include "Member.h"
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

    /// Get user information of channel members other than the currently logged-in user
    TArray<FUserRef> GetOtherMemberUsers() const;

    /// Get all the messages of this channel
    const TArray<FMessage>& GetMessages() const;

    /// Get the count of unread messages for the current user in this channel
    int32 UnreadCount() const;

    /// Type of channel, either built-in or custom
    /// @see https://getstream.io/chat/docs/other-rest/channel_features/
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel") FString Type;

    /// An identifier for this channel. Not globally unique. A subset of the Cid.
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Id;

    /// The members of this channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    TArray<FMember> Members;

    /// Number of channel watchers
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    int32 WatcherCount;

    /// The read states of each user in this channel
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    TArray<FRead> Read;

    /// The human-readable name of this channel
    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString Name;

    /// The URL of an image to show as the "profile-picture" of this channel
    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel")
    FString ImageUrl;

    /// The cid of this channel
    FString Cid;

    /// Configuration of the channel (usually set via the dashboard)
    FChannelConfig Config;

private:
    void SetMembers(UUserManager&, const TArray<FChannelMemberDto>&);
    static TArray<FMessage> Convert(const FChannelStateResponseFieldsDto&, UUserManager&);

    TArray<FMessage> Messages;
};
