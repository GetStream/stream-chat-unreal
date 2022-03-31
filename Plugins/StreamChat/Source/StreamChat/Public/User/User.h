// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MutedUser.h"

#include "User.generated.h"

struct FOwnUserDto;
struct FUserDto;
struct FUserObjectDto;
struct FUserObjectRequestDto;

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
    explicit FUser(const FOwnUserDto&, UUserManager*);
    /// Convert to UserDto
    explicit operator FUserDto() const;
    /// Convert to UserObjectDto
    explicit operator FUserObjectDto() const;
    /// Convert to UserObjectRequestDto
    explicit operator FUserObjectRequestDto() const;

    // Get name, falling back to id
    const FString& GetNameOrId() const;
    /// Generate initials from the user's name
    FString GetInitials(int32 Limit = 2) const;
    /// Return best value for when the user was last active
    const FDateTime& GetLastActive() const;
    /// e.g. Last seen 10 minutes ago
    FText GetLastSeenText() const;
    void Update(const FUser& User);

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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User", AdvancedDisplay)
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User", AdvancedDisplay)
    FDateTime UpdatedAt = FDateTime{0};

    /// Date/time of deactivation
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User", AdvancedDisplay)
    FDateTime DeactivatedAt = FDateTime{0};

    /// Date/time of deletion
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User", AdvancedDisplay)
    FDateTime DeletedAt = FDateTime{0};

    /// Date of last activity
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User", AdvancedDisplay)
    FDateTime LastActive = FDateTime{0};

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    int32 UnreadCount = 0;

    /// Only populated for current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    int32 TotalUnreadCount = 0;

    /// Only populated for current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    int32 UnreadChannels = 0;

    /// Only populated for current user
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stream Chat|User")
    TArray<FMutedUser> MutedUsers;

    /// Expiration date of the ban
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|User", AdvancedDisplay)
    FDateTime BanExpires = FDateTime{0};

    /// Whether a user is banned or not
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|User", AdvancedDisplay)
    bool bBanned = false;

    /// Preferred language of a user
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|User", AdvancedDisplay)
    FString Language;

    /// Revocation date for tokens
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|User", AdvancedDisplay)
    FDateTime RevokeTokensIssuedBefore = FDateTime{0};

    /// Determines the set of user permissions
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|User", AdvancedDisplay)
    FString Role;

    /// List of teams user is a part of
    UPROPERTY(BlueprintReadWrite, Category = "Stream Chat|User", AdvancedDisplay)
    TArray<FString> Teams;

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
     * @brief Generate a valid user ID from a human-readable name
     * @param Name User's human-readable name
     * @param OutUserId A valid UserId, with all disallowed characters replaced with '_'
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|User")
    static bool GenerateUserId(const FString& Name, FString& OutUserId);

    /**
     * @brief Ensure a human readable user name is valid by removing bad characters
     * @param Name User's human-readable name
     * @param SanitizedName A valid user name
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|User")
    static bool SanitizeName(const FString& Name, FString& SanitizedName);
};
