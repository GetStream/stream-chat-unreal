// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "Response/Message/AttachmentDto.h"

#include "MessageRequestDto.generated.h"

/**
 * @brief #/components/schemas/MessageRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/rest/#messages-sendmessage-request
 */
USTRUCT()
struct FMessageRequestDto
{
    GENERATED_BODY()

    /// Files, images and links to attach to the message.
    /// Upload the content first, then reference the returned URL here
    UPROPERTY()
    TArray<FAttachmentDto> Attachments;

    UPROPERTY()
    FString Cid;

    /// Contains HTML markup of the message
    UPROPERTY()
    FString Html;

    /// Message ID is unique string identifier of the message
    UPROPERTY()
    FString Id;

    /// IDs of the users mentioned in the message.
    /// The backend resolves these to full user objects on the way back out.
    UPROPERTY()
    TArray<FString> MentionedUsers;

    /// Should be empty if `text` is provided
    UPROPERTY()
    FString Mml;

    /// The ID of the parent message. Setting this makes the message a reply in that message's
    /// thread, and the backend turns it into a message of type `reply`.
    UPROPERTY()
    FString ParentId;

    /// Whether a thread reply should also appear in the main channel message list.
    /// Ignored by the backend unless `ParentId` is set.
    UPROPERTY()
    bool bShowInChannel = false;

    /// When the pin should lapse. Leave zeroed to pin indefinitely; ignored unless `bPinned`.
    UPROPERTY()
    FDateTime PinExpires = FDateTime{0};

    /// Whether to pin the message in the channel.
    /// `pinned_at` and `pinned_by` are not sent: the backend stamps both from this request.
    UPROPERTY()
    bool bPinned = false;

    /// The ID of the message this one quotes. The backend embeds the quoted message in responses.
    UPROPERTY()
    FString QuotedMessageId;

    /// A map describing the count of score of every reaction
    UPROPERTY()
    TMap<FName, int32> ReactionScores;

    // Whether message is silent or not
    UPROPERTY()
    bool bSilent = false;

    /// Text of the message. Should be empty if `mml` is provided
    UPROPERTY()
    FString Text;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
