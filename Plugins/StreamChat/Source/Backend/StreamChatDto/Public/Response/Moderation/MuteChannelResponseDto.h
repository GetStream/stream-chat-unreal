// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelMuteDto.h"
#include "CoreMinimal.h"
#include "OwnUserDto.h"
#include "Response/ResponseDto.h"

#include "MuteChannelResponseDto.generated.h"

/**
 * @brief #/components/schemas/MuteChannelResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMuteChannelResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// Object with channel mute (if one channel was muted)
    UPROPERTY()
    FChannelMuteDto ChannelMute;

    /// Object with mutes (if multiple channels were muted)"
    UPROPERTY()
    TArray<FChannelMuteDto> ChannelMutes;

    /// Authorized user object with fresh mutes information
    UPROPERTY()
    FOwnUserDto OwnUser;
};
