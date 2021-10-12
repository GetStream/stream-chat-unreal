// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "User.generated.h"

struct FUserDto;

/**
 * A Stream Chat user
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUser
{
    GENERATED_BODY()

    FUser() = default;
    explicit FUser(const FUserDto&);
    explicit FUser(const FString& InId);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat User")
    FString Id;
};
