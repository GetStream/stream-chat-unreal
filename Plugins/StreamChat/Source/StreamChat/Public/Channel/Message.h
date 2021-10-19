// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Reaction.h"
#include "User.h"

#include "Message.generated.h"

struct FMessageDto;
struct FMessageRequestDto;

UENUM(BlueprintType)
enum class EMessageSendState : uint8
{
    Sending,
    Sent,
    Updating,
    Deleting,
    Failed
};

UENUM(BlueprintType)
enum class EMessageType : uint8
{
    Regular,
    Ephemeral,
    Error,
    Reply,
    System,
    Deleted
};

/**
 * A Stream Chat message
 */
USTRUCT(BlueprintType)
struct FMessage
{
    GENERATED_BODY()

    FMessage() = default;
    // From server responses
    explicit FMessage(const FMessageDto&);
    // Updating
    explicit operator FMessageRequestDto() const;
    // Sending
    explicit FMessage(const FMessageRequestDto&, const FUser& SendingUser);

    /// The message ID. This is either created by Stream or set client side when
    /// the message is added.
    UPROPERTY()
    FString Id;

    /// The text of this message
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    FString Text;

    /// Sending state of the message
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    EMessageSendState State;

    /// User who sent the message
    // TODO Optional
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    FUser User;

    /// The message type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    EMessageType Type;

    /// The list of user mentioned in the message
    UPROPERTY()
    TArray<FUser> MentionedUsers;

    /// Reserved field indicating when the message was updated last time.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    FDateTime UpdatedAt;

    /// Reserved field indicating when the message was deleted.
    TOptional<FDateTime> DeletedAt;

    // Reactions

    /// A map describing the count of number of every reaction
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    TMap<FString, int32> ReactionCounts;

    /// A map describing the count of score of every reaction
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    TMap<FString, int32> ReactionScores;

    /// The latest reactions to the message created by any user.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    TArray<FReaction> LatestReactions;

    /// The reactions added to the message by the current user.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat Message")
    TArray<FReaction> OwnReactions;

    // TODO rest of fields
};
