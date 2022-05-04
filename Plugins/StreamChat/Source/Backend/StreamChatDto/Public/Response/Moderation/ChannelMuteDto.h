// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Channel/ChannelResponseDto.h"
#include "UserObjectDto.h"

#include "ChannelMuteDto.generated.h"

/**
 * @brief #/components/schemas/ChannelMute
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FChannelMuteDto
{
    GENERATED_BODY()

    // Channel who is muted
    UPROPERTY()
    FChannelResponseDto Channel;

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of mute expiration
    UPROPERTY()
    FDateTime Expires = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    // Owner of channel mute
    UPROPERTY()
    FUserObjectDto User;
};
