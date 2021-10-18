// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageTypeDto.h"
#include "UserDto.h"

#include "MessageDto.generated.h"

/**
 * Represents any chat message.
 * Used in responses only
 * #/components/schemas/Message
 */
USTRUCT()
struct FMessageDto
{
    GENERATED_BODY()

    /// The list of attachments, either provided by the user or generated from a
    /// command or as a result of URL scraping.
    // TODO Uncomment
    // UPROPERTY()
    // TArray<FAttachment> Attachments;

    /// Channel unique identifier
    UPROPERTY()
    FString Cid;

    /// Contains provided slash command
    // TODO Optional
    UPROPERTY()
    FString Command;

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt;

    /// Date/time of deletion
    UPROPERTY()
    FDateTime DeletedAt;

    /// Contains HTML markup of the message
    UPROPERTY()
    FString Html;

    // TODO is this an object?
    UPROPERTY()
    FString I18n;

    /// The message ID. This is either created by Stream or set client side when
    /// the message is added.
    UPROPERTY()
    FString Id;

    // TODO ImageLabels

    /// The latest reactions to the message created by any user.
    // TODO Uncomment
    // TODO Optional
    // UPROPERTY()
    // TArray<FReaction> LatestReactions;

    /// The list of user mentioned in the message
    UPROPERTY()
    TArray<FUserDto> MentionedUsers;

    /// Messaging Markup Language
    /// Should be empty if `text` is provided
    UPROPERTY()
    FString Mml;

    /// The reactions added to the message by the current user.
    // TODO Uncomment
    // TODO Optional
    // UPROPERTY()
    // TArray<FReaction> OwnReactions;

    /// The ID of the parent message, if the message is a thread reply.
    // TODO Optional
    UPROPERTY()
    FString ParentId;

    /// Reserved field indicating when the message will expire
    ///
    /// if `null` message has no expiry
    // TODO Optional
    UPROPERTY()
    FDateTime PinExpires;

    /// If true the message is pinned
    UPROPERTY()
    bool bPinned;

    /// Reserved field indicating when the message was pinned
    // TODO Optional
    UPROPERTY()
    FDateTime PinnedAt;

    /// Reserved field indicating who pinned the message
    // TODO Optional
    UPROPERTY()
    FUserDto PinnedBy;

    /// A quoted reply message
    // TODO Optional
    // TODO recursive?
    // UPROPERTY()
    // FMessageDto QuotedMessage;

    /// The ID of the quoted message, if the message is a quoted reply.
    // TODO Optional
    UPROPERTY()
    FString QuotedMessageId;

    /// A map describing the count of number of every reaction
    // TODO Optional
    UPROPERTY()
    TMap<FString, int32> ReactionCounts;

    /// A map describing the count of score of every reaction
    // TODO Optional
    UPROPERTY()
    TMap<FString, int32> ReactionScores;

    /// Reserved field indicating the number of replies for this message.
    // TODO Optional
    UPROPERTY()
    int ReplyCount;

    /// If true the message is shadowed
    UPROPERTY()
    bool bShadowed;

    /// Whether thread reply should be shown in the channel as well
    // TODO Optional
    UPROPERTY()
    bool bShowInChannel;

    /// If true the message is silent
    UPROPERTY()
    bool bSilent;

    /// The text of this message
    UPROPERTY()
    FString Text;

    /// Reserved field indicating the thread participants for this message.
    // TODO Optional
    UPROPERTY()
    TArray<FUserDto> ThreadParticipants;

    /// The message type
    UPROPERTY()
    EMessageTypeDto Type = EMessageTypeDto::Regular;

    /// Reserved field indicating when the message was updated last time.
    UPROPERTY()
    FDateTime UpdatedAt;

    /// User who sent the message
    // TODO Optional
    UPROPERTY()
    FUserDto User;

    // TODO extraData
};
