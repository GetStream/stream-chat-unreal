// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Message.h"
#include "Read.h"
#include "User/UserRef.h"

#include "ChatThread.generated.h"

class UUserManager;
struct FThreadStateResponseDto;

/**
 * @brief Represents a Stream Chat thread: a message and the replies hanging off it
 *
 * Returned by UStreamChatClientComponent::QueryThreads, which lists threads across every channel
 * the current user is a member of. The replies carried here are only the most recent ones, enough
 * to preview the thread; fetch the rest through the channel with UChatChannel::QueryReplies.
 *
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FChatThread
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FChatThread();

    /// Create a thread from a DTO from the API
    explicit FChatThread(const FThreadStateResponseDto&, UUserManager*);

    /// Get the count of replies in this thread which the current user hasn't read
    int32 UnreadCount() const;

    /// CID of the channel this thread belongs to
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FString ChannelCid;

    /// ID of the message which started this thread
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FString ParentMessageId;

    /// The message which started this thread
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FMessage ParentMessage;

    /// The user who started this thread
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FUserRef CreatedBy;

    /// Total number of replies in this thread
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    int32 ReplyCount = 0;

    /// Number of users who have ever replied in this thread
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    int32 ParticipantCount = 0;

    /// Number of users still participating in this thread
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    int32 ActiveParticipantCount = 0;

    /// The users participating in this thread
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    TArray<FUserRef> ThreadParticipants;

    /// The most recent replies in this thread, oldest first. Not the whole thread.
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    TArray<FMessage> LatestReplies;

    /// The read state of each participant of this thread
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    TArray<FRead> Read;

    /// Date of the most recent reply
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FDateTime LastMessageAt = FDateTime{0};

    /// Date this thread was started
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FDateTime CreatedAt = FDateTime{0};

    /// Date this thread was last updated
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FDateTime UpdatedAt = FDateTime{0};

    /// Title of this thread, defaulting server-side to the text of the parent message
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FString Title;

    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Thread")
    FAdditionalFields ExtraData;
};
