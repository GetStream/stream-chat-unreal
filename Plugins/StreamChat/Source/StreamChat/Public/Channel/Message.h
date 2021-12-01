// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
    /// Message is shown, but user might see a "pending" icon
    Sending,
    /// Message is shown, and user might see a "confirmed" icon
    Sent,
    /// Message is shown, but user might see a "pending" icon
    Updating,
    /// Message might be shown as deleted already, pending confirmation from API
    Deleting,
    /// User might see a "failed" icon
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
    /// A regular message created in the channel.
    Regular,
    /// A temporary message which is only delivered to one user. It is not stored in the channel history.
    /// Ephemeral messages are normally used by commands (e.g. `/giphy`) to prompt messages or request for actions.
    Ephemeral,
    /// An error message generated as a result of a failed command.
    /// It is also ephemeral, as it is not stored in the channel history and is only delivered to one user.
    Error,
    /// A message in a reply thread. Messages created with `parent_id` are automatically of this type.
    Reply,
    /// A message generated by a system event, like updating the channel or muting a user.
    System,
    /// A soft deleted message
    Deleted
};

/**
 * @brief Represents a Stream Chat message
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FMessage
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FMessage();
    /// Create a message from a DTO from the API
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
