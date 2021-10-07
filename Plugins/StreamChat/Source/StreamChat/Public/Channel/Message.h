// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "User.h"

#include "Message.generated.h"

struct FMessageDto;

/**
 *
 */
USTRUCT(BlueprintType)
struct FMessage
{
    GENERATED_BODY()

    FMessage() = default;
    explicit FMessage(const FMessageDto&);

    /// The message ID. This is either created by Stream or set client side when
    /// the message is added.
    UPROPERTY()
    FString Id;

    /// The text of this message
    // TODO Optional
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString Text;

    /// The message type
    UPROPERTY()
    FString Type;

    /// The list of user mentioned in the message
    UPROPERTY()
    TArray<FUser> MentionedUsers;

    /// If true the message is silent
    UPROPERTY()
    bool bSilent;

    /// If true the message is shadowed
    UPROPERTY()
    bool bShadowed;

    /// User who sent the message
    // TODO Optional
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FUser User;

    /// If true the message is pinned
    UPROPERTY()
    bool bPinned;

    // TODO rest of fields
};
