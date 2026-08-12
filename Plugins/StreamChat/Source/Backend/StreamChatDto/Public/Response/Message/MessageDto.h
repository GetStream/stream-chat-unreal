// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "MessageTypeDto.h"
#include "Response/Message/AttachmentDto.h"
#include "Response/Moderation/MessageModerationDto.h"
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
    UPROPERTY()
    TArray<FAttachmentDto> Attachments;

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

    /// Automatic translations of the message, sent only by an app with auto-translation enabled.
    ///
    /// Keyed by `<language>_text` for each translation, plus a `language` key naming the language
    /// the message was written in. Read through FMessage rather than picking those apart here.
    UPROPERTY()
    TMap<FString, FString> I18n;

    /// The message ID. This is either created by Stream or set client side when
    /// the message is added.
    UPROPERTY()
    FString Id;

    // `image_labels` is not read: it is how the older moderation API reported what it saw in an
    // attached image, and Moderation V2 sends the same thing as image harms, which FMessageModeration
    // already carries for both versions.

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

    /// The moderation verdict for this message, as sent by Moderation V2
    UPROPERTY()
    FMessageModerationDto Moderation;

    /// The moderation verdict for this message, as sent by the older moderation API
    ///
    /// Apps still on that version get their verdict here instead of in Moderation, so both are read.
    UPROPERTY()
    FMessageModerationDto ModerationDetails;

    /// The reactions added to the message by the current user.
    UPROPERTY()
    TArray<FReactionDto> OwnReactions;

    /// The ID of the parent message, if the message is a thread reply.
    UPROPERTY()
    FString ParentId;

    /// When the pin lapses. Stays zeroed when the API sends `null`, which means the pin doesn't expire.
    UPROPERTY()
    FDateTime PinExpires = FDateTime{0};

    /// If true the message is pinned
    UPROPERTY()
    bool bPinned = false;

    /// When the message was pinned. Zeroed unless it is pinned.
    UPROPERTY()
    FDateTime PinnedAt = FDateTime{0};

    /// Who pinned the message. Left with an empty ID unless it is pinned.
    UPROPERTY()
    FUserObjectDto PinnedBy;

    // The quoted message itself is not read: a USTRUCT cannot hold one of its own type, so this DTO
    // would have to be duplicated to carry it. The ID below is the handle, and the quoted message is
    // in the channel's message list under it.

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

    /// User who sent the message.
    /// Left with an empty ID by the handful of payloads which omit it, which yields an invalid
    /// FUserRef on the message rather than a reference to a user who doesn't exist.
    UPROPERTY()
    FUserObjectDto User;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
