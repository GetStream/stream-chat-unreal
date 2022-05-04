// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MuteChannelRequestDto.generated.h"

/**
 * @brief #/components/schemas/MuteChannelRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMuteChannelRequestDto
{
    GENERATED_BODY()

    /// Channel CIDs to mute (if multiple channels)
    UPROPERTY()
    TArray<FString> ChannelCids;

    /// Duration of mute in milliseconds
    UPROPERTY()
    float Expiration = TNumericLimits<float>::Max();
};
