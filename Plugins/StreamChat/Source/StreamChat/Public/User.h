// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "User.generated.h"

struct FUserObjectDto;

/**
 * A Stream Chat user
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUser
{
    GENERATED_BODY()

    FUser() = default;
    explicit FUser(const FString& InId);

    explicit FUser(const FUserObjectDto&);
    explicit operator FUserObjectDto() const;

    FString GetInitials(int32 Limit = 2) const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|User")
    FString Id;

    // Not in the API spec, but common
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|User")
    FString Name;

    // Not in the API spec, but common
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|User")
    FString Image;
};
