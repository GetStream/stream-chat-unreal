// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "OwnUserDto.generated.h"

/**
 * @brief #/components/schemas/OwnUser
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FOwnUserDto
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
    UPROPERTY(Transient)
    bool bInvisible = false;

    UPROPERTY(Transient)
    int32 TotalUnreadCount = -1;

    UPROPERTY(Transient)
    int32 UnreadChannels = -1;

    // Not in the API spec, but common
    UPROPERTY(Transient)
    FString Name;

    // Not in the API spec, but common
    UPROPERTY(Transient)
    FString Image;
};
