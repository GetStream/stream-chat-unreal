// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStateResponseFieldsDto.h"
#include "CoreMinimal.h"

#include "ChannelsResponseDto.generated.h"

/**
 * @brief #/components/schemas/ChannelsResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FChannelsResponseDto
{
    GENERATED_BODY()

    /// List of channels
    UPROPERTY()
    TArray<FChannelStateResponseFieldsDto> Channels;

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;
};
