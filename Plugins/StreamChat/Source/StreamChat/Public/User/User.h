// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

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

    /// Whether a user is online or not
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|User")
    bool bOnline = false;

    /// When invisible is set to true, the current user will appear as offline to other users.
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|User", AdvancedDisplay)
    bool bInvisible = false;

    /// Date/time of creation
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|User")
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY(BlueprintReadOnly, Category = "Stream Chat|User")
    FDateTime UpdatedAt = FDateTime{0};

    /// Date of last activity
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    FDateTime LastActive = FDateTime{0};

    /// The human-readable name of the user
    // Not in the API spec, but common
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat|User")
    FString Name;

    /// The URL of a profile picture of the user
    // Not in the API spec, but common
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat|User")
    FString Image;
};

/**
 * @brief Blueprint functions for the User struct
 * @ingroup StreamChat
 */
UCLASS()
class STREAMCHAT_API UUserBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * @brief Generated a valid user ID from a human-readable name
     * @param Name User's human-readable name
     * @return A valid UserId, with all disallowed characters replaced with '_'
     */
    UFUNCTION(BlueprintPure, Category = "Stream Chat|User")
    static FString GenerateUserId(const FString& Name);
};
