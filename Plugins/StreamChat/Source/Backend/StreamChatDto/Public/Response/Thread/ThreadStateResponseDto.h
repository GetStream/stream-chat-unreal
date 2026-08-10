// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "Response/Channel/ChannelResponseDto.h"
#include "Response/Channel/ReadDto.h"
#include "Response/Message/MessageDto.h"
#include "Response/Thread/ThreadParticipantDto.h"
#include "UserObjectDto.h"

#include "ThreadStateResponseDto.generated.h"

/**
 * @brief #/components/schemas/ThreadStateResponse
 * An enriched thread: the parent message plus its latest replies, participants and read states.
 * Used in responses only
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FThreadStateResponseDto
{
    GENERATED_BODY()

    /// CID of the channel this thread belongs to
    UPROPERTY()
    FString ChannelCid;

    /// The channel this thread belongs to
    UPROPERTY()
    FChannelResponseDto Channel;

    /// ID of the message which started the thread
    UPROPERTY()
    FString ParentMessageId;

    /// The message which started the thread
    UPROPERTY()
    FMessageDto ParentMessage;

    /// ID of the user who started the thread
    UPROPERTY()
    FString CreatedByUserId;

    /// The user who started the thread
    UPROPERTY()
    FUserObjectDto CreatedBy;

    /// Number of replies in this thread
    UPROPERTY()
    int32 ReplyCount = 0;

    /// Number of users who have ever participated in this thread
    UPROPERTY()
    int32 ParticipantCount = 0;

    /// Number of users who are still participating in this thread
    UPROPERTY()
    int32 ActiveParticipantCount = 0;

    /// The users participating in this thread
    UPROPERTY()
    TArray<FThreadParticipantDto> ThreadParticipants;

    /// The most recent replies in this thread, oldest first
    UPROPERTY()
    TArray<FMessageDto> LatestReplies;

    /// The read state of each participant of this thread
    UPROPERTY()
    TArray<FReadDto> Read;

    /// Date/time of the last reply
    UPROPERTY()
    FDateTime LastMessageAt = FDateTime{0};

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    /// Date/time of deletion
    UPROPERTY()
    FDateTime DeletedAt = FDateTime{0};

    /// Title of the thread
    UPROPERTY()
    FString Title;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
