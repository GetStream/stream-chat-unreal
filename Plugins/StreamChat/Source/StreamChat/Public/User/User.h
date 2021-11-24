// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "User.generated.h"

struct FOwnUserDto;
struct FUserObjectDto;

/**
 * @brief A Stream Chat user
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUser
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FUser();

    /// Create a new sparsely-identified user for local use
    explicit FUser(const FString& InId);

    /// Convert from UserObject
    explicit FUser(const FUserObjectDto&);
    /// Convert from OwnUser
    explicit FUser(const FOwnUserDto&);
    /// Convert to UserObject
    explicit operator FUserObjectDto() const;

    /// Generate initials from the user's name
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
