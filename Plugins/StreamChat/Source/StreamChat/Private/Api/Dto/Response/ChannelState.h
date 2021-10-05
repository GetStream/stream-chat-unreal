// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelModel.h"
#include "CoreMinimal.h"
#include "Message.h"

#include "ChannelState.generated.h"

/**
 *
 */
USTRUCT()
struct FChannelState
{
	GENERATED_BODY()

	/// The channel to which this state belongs
	UPROPERTY()
	FChannelModel Channel;

	/// A paginated list of channel messages
	UPROPERTY()
	TArray<FMessage> Messages;

	/// A paginated list of channel members
	// TODO Uncomment
	// UPROPERTY()
	// TArray<FMember> Members;

	/// A paginated list of pinned messages
	UPROPERTY()
	TArray<FMessage> PinnedMessages;

	/// The count of users watching the channel
	UPROPERTY()
	int32 WatcherCount;

	/// A paginated list of users watching the channel
	UPROPERTY()
	TArray<FUser> Watchers;

	/// The list of channel reads
	// TODO Uncomment
	// UPROPERTY()
	// TArray<FRead> Read;
};
