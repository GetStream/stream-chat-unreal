// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelModelDto.h"
#include "CoreMinimal.h"
#include "MessageDto.h"

#include "ChannelStateResponseDto.generated.h"

/**
 *
 */
USTRUCT()
struct FChannelStateResponseDto
{
    GENERATED_BODY()

    /// The channel to which this state belongs
    UPROPERTY()
    FChannelModelDto Channel;

    /// A paginated list of channel messages
    UPROPERTY()
    TArray<FMessageDto> Messages;

    /// A paginated list of channel members
    // TODO Uncomment
    // UPROPERTY()
    // TArray<FMember> Members;

    /// A paginated list of pinned messages
    UPROPERTY()
    TArray<FMessageDto> PinnedMessages;

    /// The count of users watching the channel
    UPROPERTY()
    int32 WatcherCount;

    /// A paginated list of users watching the channel
    UPROPERTY()
    TArray<FUserDto> Watchers;

    /// The list of channel reads
    // TODO Uncomment
    // UPROPERTY()
    // TArray<FRead> Read;
};
