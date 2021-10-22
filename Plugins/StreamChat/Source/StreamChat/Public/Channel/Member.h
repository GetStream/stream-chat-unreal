#pragma once

#include "CoreMinimal.h"
#include "User.h"

#include "Member.generated.h"

struct FChannelMemberDto;

/**
 * Contains information about a member of a chat channel
 */
USTRUCT(BlueprintType)
struct FMember
{
    GENERATED_BODY()

    FMember() = default;
    explicit FMember(const FChannelMemberDto&);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Member")
    FUser User;

    // TODO rest of the fields
};
