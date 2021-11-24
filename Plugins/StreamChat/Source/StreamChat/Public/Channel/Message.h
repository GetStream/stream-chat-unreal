// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Reaction/Reactions.h"
#include "Response/Channel/ChannelStateResponseFieldsDto.h"
#include "User/UserRef.h"

#include "Message.generated.h"

struct FMessageDto;
struct FMessageRequestDto;

/**
 * @brief The client-side state of a message
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class EMessageSendState : uint8
{
    Sending,
    Sent,
    Updating,
    Deleting,
    Failed
};

/**
 * @brief Type of message, as set by the API or chat bots and custom commands.
 * @see https://getstream.io/chat/docs/other-rest/message_format/#message-types
 * @ingroup StreamChat
 */
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
 * Represents a Stream Chat message
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FMessage
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FMessage();
    /// Convert a message given API responses
    explicit FMessage(FUserManager&, const FMessageDto&);
    /// Convert a message into a update request for sending to the API
    explicit operator FMessageRequestDto() const;
    /// Convert a message from a local request *before* sending it to the API
    explicit FMessage(const FMessageRequestDto&, const FUserRef& SendingUser);

    /// The message ID. This is either created by Stream or set client side when
    /// the message is added.
    UPROPERTY()
    FString Id;

    /// The text of this message
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    FString Text;

    /// Sending state of the message
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    EMessageSendState State;

    /// User who sent the message
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    FUserRef User;

    /// The message type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    EMessageType Type;

    /// The list of user mentioned in the message
    UPROPERTY()
    TArray<FUserRef> MentionedUsers;

    /// Date of first message creation
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    FDateTime CreatedAt;

    /// Reserved field indicating when the message was updated last time.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    FDateTime UpdatedAt;

    /// Reserved field indicating when the message was deleted.
    TOptional<FDateTime> DeletedAt;

    /// All reactions of this message, grouped by their type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    FReactions Reactions;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Message")
    bool bIsRead;

    // TODO rest of fields
};
