// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserMuteDto.h"

#include "OwnUserDto.generated.h"

/**
 * @brief #/components/schemas/OwnUser
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FOwnUserDto : public FUserDto
{
    GENERATED_BODY()

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

    UPROPERTY(Transient)
    int32 UnreadCount = -1;

    UPROPERTY(Transient)
    int32 TotalUnreadCount = -1;

    UPROPERTY(Transient)
    int32 UnreadChannels = -1;

    // Muted users
    UPROPERTY(Transient)
    TArray<FUserMuteDto> Mutes;
};
