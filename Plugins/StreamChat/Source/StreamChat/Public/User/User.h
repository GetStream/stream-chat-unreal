// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "User.generated.h"

struct FOwnUserDto;
struct FUserObjectDto;

/**
 * A Stream Chat user
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUser
{
    GENERATED_BODY()

    FUser();
    explicit FUser(const FString& InId);

    explicit FUser(const FUserObjectDto&);
    explicit FUser(const FOwnUserDto&);
    explicit operator FUserObjectDto() const;

    FString GetInitials(int32 Limit = 2) const;

    /// The id of this user
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat|User")
    FString Id;

    /// Whether a user online or not
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|User")
    bool bOnline;

    /// Date/time of creation
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|User")
    FDateTime CreatedAt;

    /// Date/time of the last update
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|User")
    FDateTime UpdatedAt;

    /// Date of last activity
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    FDateTime LastActive;

    /// The human-readable name of the user
    // Not in the API spec, but common
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|User")
    FString Name;

    /// The URL of a profile picture of the user
    // Not in the API spec, but common
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|User")
    FString Image;
};
