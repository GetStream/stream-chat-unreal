// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Moderation/MessageModerationDto.h"

#include "MessageModeration.generated.h"

/**
 * @brief What moderation decided to do with a message
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class EMessageModerationAction : uint8
{
    /// Moderation reached no verdict, which is the case for almost every message
    None,
    /// The message needs rephrasing and sending again. It was neither published nor stored.
    Bounce,
    /// The message was blocked and removed from the channel.
    Remove,
    /// The message was published, but sent to the dashboard for a moderator to review.
    Flag,
    /// An action this version of the SDK does not know about. Read FMessageModeration::RawAction.
    Other
};

/**
 * @brief The moderation verdict for a message
 *
 * Populated from the `moderation` field of Moderation V2, falling back to the older
 * `moderation_details` field, whose action names are translated to the same enum.
 *
 * Only Action and OriginalText come back on a client-side response. The harm labels below are
 * included in server-side responses only, so expect them to be empty in a game.
 *
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FMessageModeration
{
    GENERATED_BODY()

    FMessageModeration() = default;
    explicit FMessageModeration(const FMessageModerationDto&);

    /// Did moderation reach a verdict on this message at all?
    bool IsSet() const;

    /// What moderation did with the message
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    EMessageModerationAction Action = EMessageModerationAction::None;

    /**
     * @brief The action exactly as the API named it
     *
     * The only way to tell one Other action from another, and the only way to read an action added
     * to the API after this version of the SDK was built.
     */
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FString RawAction;

    /// The text the user submitted, before moderation replaced or withheld it
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FString OriginalText;

    /// Harm labels the text triggered. Server-side responses only.
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    TArray<FString> TextHarms;

    /// Harm labels the images triggered. Server-side responses only.
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    TArray<FString> ImageHarms;

    /// Name of the blocklist the message matched, if any. Server-side responses only.
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FString BlocklistMatched;

    /// The semantic filter phrase the message matched, if any. Server-side responses only.
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    FString SemanticFilterMatched;

    /// Whether the message tripped the platform circumvention model. Server-side responses only.
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Moderation")
    bool bPlatformCircumvented = false;
};
