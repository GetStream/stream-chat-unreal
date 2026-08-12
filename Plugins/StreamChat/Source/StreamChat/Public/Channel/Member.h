// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "User/UserRef.h"

#include "Member.generated.h"

class FUserManager;
struct FChannelMemberDto;

/**
 * @brief Contains information about a member of a chat channel
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FMember
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FMember();
    /// Create a member from a user
    explicit FMember(const FUserRef& User);
    /// Create a member from a DTO from the API
    explicit FMember(const FChannelMemberDto&, UUserManager*);

    bool operator==(const FUserRef&) const;
    bool operator==(const FMember&) const;
    friend uint32 GetTypeHash(const FMember& M);

    /**
     * @brief Has this member been invited to the channel and not yet answered?
     *
     * Both answers are recorded as a date, so an invite is outstanding only while neither is set.
     * Show such a member an accept/reject prompt rather than the channel itself.
     */
    bool IsInvitePending() const;

    /// Has this member accepted their invitation to the channel?
    bool HasAcceptedInvite() const;

    /// Has this member rejected their invitation to the channel?
    bool HasRejectedInvite() const;

    /// The user information of this member
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member") FUserRef User;

    /// Expiration date of the ban
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FDateTime BanExpires = FDateTime{0};

    /// Whether member is banned in this channel or not
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    bool bBanned = false;

    /// Role of the member in the channel
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FString ChannelRole;

    /// When this user became a member of the channel
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FDateTime CreatedAt = FDateTime{0};

    /// When this membership was last updated
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FDateTime UpdatedAt = FDateTime{0};

    /// Whether this member was invited to the channel, rather than simply added to it
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    bool bInvited = false;

    /// When this member accepted their invitation. Zero if they never did.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FDateTime InviteAcceptedAt = FDateTime{0};

    /// When this member rejected their invitation. Zero if they never did.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FDateTime InviteRejectedAt = FDateTime{0};

    /// Whether this member can moderate the channel
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    bool bIsModerator = false;

    /**
     * @brief Whether this member is shadow banned in this channel
     *
     * Their messages arrive marked as shadowed for everybody else, and not at all for a client
     * which hides them. They are not told about any of this, which is the point.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    bool bShadowBanned = false;

    /// Whether this member muted notifications from the channel
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    bool bNotificationsMuted = false;

    /// When this member archived the channel for themselves. Zero if they have not.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FDateTime ArchivedAt = FDateTime{0};

    /// When this member pinned the channel for themselves. Zero if they have not.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FDateTime PinnedAt = FDateTime{0};

    /// Any custom fields set on the membership
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|Channel|Member", AdvancedDisplay)
    FAdditionalFields ExtraData;
};
