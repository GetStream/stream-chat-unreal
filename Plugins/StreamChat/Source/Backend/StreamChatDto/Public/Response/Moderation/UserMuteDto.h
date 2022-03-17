// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "UserMuteDto.generated.h"

/**
 * @brief #/components/schemas/UserMute
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUserMuteDto
{
    GENERATED_BODY()

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of mute expiration
    UPROPERTY()
    FDateTime Expires = FDateTime{0};

    // User who's muted
    UPROPERTY()
    FUserObjectDto Target;

    /// Date/time of the last update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    // Owner of mute
    UPROPERTY()
    FUserObjectDto User;
};
