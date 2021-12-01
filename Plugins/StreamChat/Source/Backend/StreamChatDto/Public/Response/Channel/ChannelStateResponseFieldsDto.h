// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelMemberDto.h"
#include "ChannelResponseDto.h"
#include "CoreMinimal.h"
#include "ReadDto.h"
#include "Response/Message/MessageDto.h"

#include "ChannelStateResponseFieldsDto.generated.h"

/**
 * @brief #/components/schemas/ChannelStateResponseFields
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FChannelStateResponseFieldsDto
{
    GENERATED_BODY()

    UPROPERTY()
    FChannelResponseDto Channel;

    /// Whether this channel is hidden or not
    UPROPERTY()
    bool bHidden;

    /// Messages before this date are hidden from the user
    UPROPERTY()
    FDateTime HideMessagesBefore;

    /// List of channel members
    UPROPERTY()
    TArray<FChannelMemberDto> Members;

    /// Current user membership object
    UPROPERTY()
    FChannelMemberDto Membership;

    /// List of channel members
    UPROPERTY()
    TArray<FMessageDto> Messages;

    /// List of pinned messages in the channel
    UPROPERTY()
    TArray<FMessageDto> PinnedMessages;

    /// List of read states
    UPROPERTY()
    TArray<FReadDto> Read;

    /// The count of users watching the channel
    UPROPERTY()
    int32 WatcherCount;

    /// A list of users watching the channel
    UPROPERTY()
    TArray<FUserObjectDto> Watchers;
};
