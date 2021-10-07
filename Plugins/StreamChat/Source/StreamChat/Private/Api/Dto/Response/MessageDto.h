// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserDto.h"

#include "MessageDto.generated.h"

/**
 * Represents any chat message.
 * Used in responses only
 */
USTRUCT()
struct FMessageDto
{
    GENERATED_BODY()

    /// The message ID. This is either created by Stream or set client side when
    /// the message is added.
    UPROPERTY()
    FString Id;

    /// The text of this message
    // TODO Optional
    UPROPERTY()
    FString Text;

    /// The message type
    UPROPERTY()
    // TODO enum
    FString Type = TEXT("regular");

    /// The list of attachments, either provided by the user or generated from a
    /// command or as a result of URL scraping.
    // TODO Uncomment
    // UPROPERTY()
    // TArray<FAttachment> Attachments;

    /// The list of user mentioned in the message
    UPROPERTY()
    TArray<FUserDto> MentionedUsers;

    /// A map describing the count of number of every reaction
    // TODO Optional
    UPROPERTY()
    TMap<FString, int32> ReactionCounts;

    /// A map describing the count of score of every reaction
    // TODO Optional
    UPROPERTY()
    TMap<FString, int32> ReactionScores;

    /// The latest reactions to the message created by any user.
    // TODO Uncomment
    // TODO Optional
    // UPROPERTY()
    // TArray<FReaction> LatestReactions;

    /// The reactions added to the message by the current user.
    // TODO Uncomment
    // TODO Optional
    // UPROPERTY()
    // TArray<FReaction> OwnReactions;

    /// The ID of the parent message, if the message is a thread reply.
    // TODO Optional
    UPROPERTY()
    FString ParentId;

    /// A quoted reply message
    // TODO Optional
    // TODO recursive?
    // UPROPERTY()
    // FMessageDto QuotedMessage;

    /// The ID of the quoted message, if the message is a quoted reply.
    // TODO Optional
    UPROPERTY()
    FString QuotedMessageId;

    /// Reserved field indicating the number of replies for this message.
    // TODO Optional
    UPROPERTY()
    int ReplyCount;

    /// Reserved field indicating the thread participants for this message.
    // TODO Optional
    UPROPERTY()
    TArray<FUserDto> ThreadParticipants;

    /// Check if this message needs to show in the channel.
    // TODO Optional
    UPROPERTY()
    bool bShowInChannel;

    /// If true the message is silent
    UPROPERTY()
    bool bSilent;

    /// If true the message is shadowed
    UPROPERTY()
    bool bShadowed;

    /// A used command name.
    // TODO Optional
    UPROPERTY()
    FString Command;

    /// Reserved field indicating when the message was created.
    UPROPERTY()
    FDateTime CreatedAt;

    /// Reserved field indicating when the message was updated last time.
    UPROPERTY()
    FDateTime UpdatedAt;

    /// User who sent the message
    // TODO Optional
    UPROPERTY()
    FUserDto User;

    /// If true the message is pinned
    UPROPERTY()
    bool bPinned;

    /// Reserved field indicating when the message was pinned
    // TODO Optional
    UPROPERTY()
    FDateTime PinnedAt;

    /// Reserved field indicating when the message will expire
    ///
    /// if `null` message has no expiry
    // TODO Optional
    UPROPERTY()
    FDateTime PinExpires;

    /// Reserved field indicating who pinned the message
    // TODO Optional
    UPROPERTY()
    FUserDto PinnedBy;

    // TODO extraData
};
