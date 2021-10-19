// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MessageRequestDto.generated.h"

/**
 * #/components/schemas/MessageRequest
 * @see https://getstream.io/chat/docs/rest/#messages-sendmessage-request
 */
USTRUCT()
struct FMessageRequestDto
{
    GENERATED_BODY()

    // TODO Attachments

    UPROPERTY()
    FString Cid;

    /// Contains HTML markup of the message
    UPROPERTY()
    FString Html;

    /// Message ID is unique string identifier of the message
    UPROPERTY()
    FString Id;

    // TODO Mentioned users

    /// Should be empty if `text` is provided
    UPROPERTY()
    FString Mml;

    // TODO ParentId, bShowInChannel

    // TODO PinExpires, bPinned, PinnedAt, PinnedBy

    // TODO quoted_message_id

    /// A map describing the count of score of every reaction
    UPROPERTY()
    TMap<FString, int32> ReactionScores;

    // Whether message is silent or not
    UPROPERTY()
    bool bSilent;

    /// Text of the message. Should be empty if `mml` is provided
    UPROPERTY()
    FString Text;
};
