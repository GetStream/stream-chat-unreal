// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OwnUserDto.h"
#include "Response/ResponseDto.h"
#include "UserMuteDto.h"

#include "MuteUserResponseDto.generated.h"

/**
 * @brief #/components/schemas/MuteUserResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMuteUserResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// Object with user mute (if one user was muted)
    UPROPERTY()
    FUserMuteDto Mute;

    /// Object with mutes (if multiple users were muted)"
    UPROPERTY()
    TArray<FUserMuteDto> Mutes;

    /// Authorized user object with fresh mutes information
    UPROPERTY()
    FOwnUserDto OwnUser;
};
