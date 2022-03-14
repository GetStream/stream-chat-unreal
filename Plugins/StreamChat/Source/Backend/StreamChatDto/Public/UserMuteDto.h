// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "UserMuteDto.generated.h"

/**
 * @brief #/components/schemas/UserMute
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FUserMuteDto
{
    GENERATED_BODY()

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of mute expiration
    UPROPERTY()
    FDateTime Expires = FDateTime{0};

    /// User who's muted
    UPROPERTY()
    FUserObjectDto Target;

    /// Date/time of the last update
    UPROPERTY(Transient)
    FDateTime UpdatedAt = FDateTime{0};

    /// Owner of channel mute
    UPROPERTY()
    FUserObjectDto User;
};
