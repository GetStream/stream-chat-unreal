// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "MessageTypeDto.h"
#include "Response/Reaction/ReactionDto.h"
#include "UserObjectDto.h"

#include "MessageDto.generated.h"

class FJsonObject;

/**
 * @brief #/components/schemas/Message
 * Represents any chat message.
 * Used in responses only
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FMessageDto
{
    GENERATED_BODY()

    /// Date/time of deletion
    TOptional<FDateTime> GetDeletedAt() const;

    /// The list of attachments, either provided by the user or generated from a
    /// command or as a result of URL scraping.
    // TODO Uncomment
    // UPROPERTY()
    // TArray<FAttachment> Attachments;

    /// Channel unique identifier
    UPROPERTY()
    FString Cid;

    /// Contains provided slash command
    UPROPERTY()
    FString Command;

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

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
    UPROPERTY()
    TArray<FReactionDto> LatestReactions;

    /// The list of user mentioned in the message
    UPROPERTY()
    TArray<FUserObjectDto> MentionedUsers;

    /// Messaging Markup Language
    /// Should be empty if `text` is provided
    UPROPERTY()
    FString Mml;

    /// The reactions added to the message by the current user.
    UPROPERTY()
    TArray<FReactionDto> OwnReactions;

    /// The ID of the parent message, if the message is a thread reply.
    UPROPERTY()
    FString ParentId;

    /// Reserved field indicating when the message will expire
    ///
    /// if `null` message has no expiry
    // TODO Optional
    UPROPERTY()
    FDateTime PinExpires = FDateTime{0};

    /// If true the message is pinned
    UPROPERTY()
    bool bPinned = false;

    /// Reserved field indicating when the message was pinned
    // TODO Optional
    UPROPERTY()
    FDateTime PinnedAt = FDateTime{0};

    /// Reserved field indicating who pinned the message
    // TODO Optional
    UPROPERTY()
    FUserObjectDto PinnedBy;

    /// A quoted reply message
    // TODO Optional
    // TODO recursive?
    // UPROPERTY()
    // FMessageDto QuotedMessage;

    /// The ID of the quoted message, if the message is a quoted reply.
    UPROPERTY()
    FString QuotedMessageId;

    /// A map describing the count of number of every reaction
    UPROPERTY()
    TMap<FName, int32> ReactionCounts;

    /// A map describing the count of score of every reaction
    UPROPERTY()
    TMap<FName, int32> ReactionScores;

    /// Reserved field indicating the number of replies for this message.
    UPROPERTY()
    uint32 ReplyCount = 0;

    /// If true the message is shadowed
    UPROPERTY()
    bool bShadowed = false;

    /// Whether thread reply should be shown in the channel as well
    UPROPERTY()
    bool bShowInChannel = false;

    /// If true the message is silent
    UPROPERTY()
    bool bSilent = false;

    /// The text of this message
    UPROPERTY()
    FString Text;

    /// Reserved field indicating the thread participants for this message.
    UPROPERTY()
    TArray<FUserObjectDto> ThreadParticipants;

    /// The message type
    UPROPERTY()
    EMessageTypeDto Type = EMessageTypeDto::Regular;

    /// Reserved field indicating when the message was updated last time.
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    /// User who sent the message
    // TODO Optional
    UPROPERTY()
    FUserObjectDto User;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
