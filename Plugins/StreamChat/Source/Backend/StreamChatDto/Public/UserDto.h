// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"

#include "UserDto.generated.h"

/**
 * @brief Common fields for users
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FUserDto
{
    GENERATED_BODY()

    explicit FUserDto() = default;
    explicit FUserDto(
        const FString& Id,
        const FDateTime& BanExpires = FDateTime{0},
        const bool bBanned = false,
        const bool bInvisible = false,
        const FString& Language = {},
        const FDateTime& RevokeTokensIssuedBefore = FDateTime{0},
        const FString& Role = {},
        const TArray<FString>& Teams = {},
        const FAdditionalFields& AdditionalFields = {})
        : Id(Id)
        , BanExpires(BanExpires)
        , bBanned(bBanned)
        , bInvisible(bInvisible)
        , Language(Language)
        , RevokeTokensIssuedBefore(RevokeTokensIssuedBefore)
        , Role(Role)
        , Teams(Teams)
        , AdditionalFields(AdditionalFields)
    {
    }

    /// Unique user identifier
    UPROPERTY()
    FString Id;

    /// Expiration date of the ban
    UPROPERTY()
    FDateTime BanExpires = FDateTime{0};

    /// Whether a user is banned or not
    UPROPERTY()
    bool bBanned = false;

    /// Whether a user should appear online or not
    UPROPERTY()
    bool bInvisible = false;

    /// Preferred language of a user
    UPROPERTY()
    FString Language;

    /// Revocation date for tokens
    UPROPERTY()
    FDateTime RevokeTokensIssuedBefore = FDateTime{0};

    /// Determines the set of user permissions
    UPROPERTY()
    FString Role;

    /// List of teams user is a part of
    UPROPERTY()
    TArray<FString> Teams;

    UPROPERTY(Transient)
    FAdditionalFields AdditionalFields;
};
