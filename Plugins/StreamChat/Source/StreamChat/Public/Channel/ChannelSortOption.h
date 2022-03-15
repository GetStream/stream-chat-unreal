// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ChannelSortOption.generated.h"

struct FSortParamRequestDto;

/**
 * @brief The desired sort direction for Stream Chat API queries
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class ESortDirection : uint8
{
    Ascending,
    Descending
};

/**
 * @brief The desired sort field for Channel API queries
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class EChannelSortField : uint8
{
    LastUpdated,
    LastMessageAt,
    UpdatedAt,
    CreatedAt,
    MemberCount,
    UnreadCount,
    HasUnread,
};

/**
 * @brief The desired sort options for Channel API queries
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FChannelSortOption
{
    GENERATED_BODY()

    /// Convert into a DTO for sending to the API
    explicit operator FSortParamRequestDto() const;

    /// Which field to sort by
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    EChannelSortField Field = EChannelSortField::LastMessageAt;

    /// The direction in which to sort
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    ESortDirection Direction = ESortDirection::Descending;
};

/**
 * @brief The desired sort field for message queries
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class EMessageSortField : uint8
{
    /// Message ID
    Id,
    /// Text of the message
    Text,
    /// Type of the message
    Type,
    /// The parent message ID (if the message is a reply)
    ParentId,
    /// The number of replies the message has
    ReplyCount,
    /// Whether or not the message contains an attachment
    Attachments,
    /// The type of the attachment
    AttachmentsType,
    /// The user ID that is included in the messages's mentions
    MentionedUsersId,
    /// The user ID of the user that sent the message
    UserId,
    /// Created at time
    CreatedAt,
    /// Updated at time
    UpdatedAt,
    /// Whether the message has been pinned,
    Pinned,
    /// Any custom field that is attached to your message. Must fill CustomField.
    Custom,
};

/**
 * @brief The desired sort options for message queries
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FMessageSortOption
{
    GENERATED_BODY()

    /// Convert into a DTO for sending to the API
    explicit operator FSortParamRequestDto() const;

    /// Which field to sort by
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    EMessageSortField Field = EMessageSortField::CreatedAt;

    /// Sort by this custom field. Only considered if "Field" is custom
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    FName CustomField;

    /// The direction in which to sort
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    ESortDirection Direction = ESortDirection::Descending;
};

/**
 * @brief The desired sort field for user queries
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class EUserSortField : uint8
{
    /// User ID
    Id,
    /// Created at time
    CreatedAt,
    /// Updated at time
    UpdatedAt,
    /// When the user was last active
    LastActive,
    /// Any custom field that is attached to your message. Must fill CustomField.
    Custom,
};

/**
 * @brief The desired sort options for user queries
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FUserSortOption
{
    GENERATED_BODY()

    /// Convert into a DTO for sending to the API
    explicit operator FSortParamRequestDto() const;

    /// Which field to sort by
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    EUserSortField Field = EUserSortField::CreatedAt;

    /// Sort by this custom field. Only considered if "Field" is custom
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    FName CustomField;

    /// The direction in which to sort
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    ESortDirection Direction = ESortDirection::Descending;
};

/**
 * @brief The desired sort field for ban queries
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class EBanSortField : uint8
{
    /// The channel CID for the ban. When this parameter is not provided, both global and channel bans will be returned.
    ChannelCid,
    /// The ID of the banned user
    UserId,
    /// The date of the ban expiration
    Expired,
    /// The date of the ban creation
    CreatedAt,
    /// The ID of the user that created the ban
    BannedById,
};

/**
 * @brief The desired sort options for ban queries
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FBanSortOption
{
    GENERATED_BODY()

    /// Convert into a DTO for sending to the API
    explicit operator FSortParamRequestDto() const;

    /// Which field to sort by
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    EBanSortField Field = EBanSortField::CreatedAt;

    /// The direction in which to sort
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    ESortDirection Direction = ESortDirection::Descending;
};
