// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Channel/ChannelResponseDto.h"
#include "UserObjectDto.h"

#include "BanResponseDto.generated.h"

/**
 * @brief #/components/schemas/BanResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FBanResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FUserObjectDto BannedBy;

    UPROPERTY()
    FChannelResponseDto Channel;

    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    UPROPERTY()
    FDateTime Expires = FDateTime{0};

    UPROPERTY()
    bool bShadow = false;

    UPROPERTY()
    FUserObjectDto User;
};
