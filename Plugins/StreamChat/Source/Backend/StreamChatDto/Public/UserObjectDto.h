// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UserObjectDto.generated.h"

/**
 * @brief #/components/schemas/UserObject
 * Used in both requests and responses
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FUserObjectDto
{
    GENERATED_BODY()

    UPROPERTY()
    FString Id;

    /// Whether a user online or not
    UPROPERTY(Transient)
    bool bOnline = false;

    /// Date/time of creation
    UPROPERTY(Transient)
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY(Transient)
    FDateTime UpdatedAt = FDateTime{0};

    /// Date of last activity
    UPROPERTY(Transient)
    FDateTime LastActive = FDateTime{0};

    /// Whether a user should appear online or not
    UPROPERTY()
    bool bInvisible = false;

    // Not in the API spec, but common
    UPROPERTY()
    FString Name;

    // Not in the API spec, but common
    UPROPERTY()
    FString Image;

    // TODO rest of the fields
};
