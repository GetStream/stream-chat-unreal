// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "User.h"

#include "Message.generated.h"

struct FMessageDto;
struct FMessageRequestDto;

UENUM(BlueprintType)
enum class EMessageSendState : uint8
{
    Sending,
    Sent
    // TODO updating, deleting, failure?
};

/**
 * A Stream Chat message
 */
USTRUCT(BlueprintType)
struct FMessage
{
    GENERATED_BODY()

    FMessage() = default;
    explicit FMessage(const FMessageRequestDto&, const FUser& SendingUser);
    explicit FMessage(const FMessageDto&);

    /// The message ID. This is either created by Stream or set client side when
    /// the message is added.
    UPROPERTY()
    FString Id;

    /// The text of this message
    // TODO Optional
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    FString Text;

    /// Sending state of the message
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    EMessageSendState State;

    /// The message type
    UPROPERTY()
    FString Type;

    /// The list of user mentioned in the message
    UPROPERTY()
    TArray<FUser> MentionedUsers;

    /// User who sent the message
    // TODO Optional
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    FUser User;

    /// Reserved field indicating when the message was updated last time.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    FDateTime UpdatedAt;

    // TODO rest of fields
};
