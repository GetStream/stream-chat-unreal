// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MessageModerationDto.generated.h"

/**
 * @brief #/components/schemas/MessageModerationResult
 * The moderation verdict the API reached for a message.
 *
 * Carries both the `moderation` key of Moderation V2 and the older `moderation_details` key: the two
 * payloads have the same shape, and only their `action` vocabulary differs.
 *
 * @ingroup StreamChatDto
 * @see https://getstream.io/moderation/docs/guides/bounce-message/
 */
USTRUCT()
struct STREAMCHATDTO_API FMessageModerationDto
{
    GENERATED_BODY()

    /**
     * What moderation did with the message.
     *
     * Left as a string rather than an enum because the server may add actions the SDK has not been
     * taught yet, and an unknown value must reach the caller instead of failing to parse. V2 sends
     * `bounce`, `remove` or `flag`; V1 sends `MESSAGE_RESPONSE_ACTION_BOUNCE` and friends.
     */
    UPROPERTY()
    FString Action;

    /// The text the user submitted, before moderation replaced or withheld it
    UPROPERTY()
    FString OriginalText;

    /// Harm labels the text triggered. Server-side responses only.
    UPROPERTY()
    TArray<FString> TextHarms;

    /// Harm labels the images triggered. Server-side responses only.
    UPROPERTY()
    TArray<FString> ImageHarms;

    /// Name of the blocklist the message matched, if any. Server-side responses only.
    UPROPERTY()
    FString BlocklistMatched;

    /// The semantic filter phrase the message matched, if any. Server-side responses only.
    UPROPERTY()
    FString SemanticFilterMatched;

    /// Whether the message tripped the platform circumvention model. Server-side responses only.
    UPROPERTY()
    bool bPlatformCircumvented = false;
};
