// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelResponseDto.h"
#include "CoreMinimal.h"

#include "DeleteChannelResponseDto.generated.h"

/**
 * @brief #/components/schemas/DeleteChannelResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FDeleteChannelResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FChannelResponseDto Channel;

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;
};
