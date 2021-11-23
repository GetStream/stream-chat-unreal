// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "User/UserRef.h"

#include "Member.generated.h"

class FUserManager;
struct FChannelMemberDto;

/**
 * Contains information about a member of a chat channel
 */
USTRUCT(BlueprintType)
struct FMember
{
    GENERATED_BODY()

    FMember() = default;
    explicit FMember(const FUserRef& User);
    explicit FMember(FUserManager&, const FChannelMemberDto&);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FUserRef User;

    // TODO rest of the fields
};
