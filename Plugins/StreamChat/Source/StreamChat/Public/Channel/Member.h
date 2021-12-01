// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
    explicit FMember(FUserManager&, const FChannelMemberDto&);

    /// The user information of this member
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FUserRef User;

    // TODO rest of the fields
};
