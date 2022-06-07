// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

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

    // TODO rest of the fields
};
