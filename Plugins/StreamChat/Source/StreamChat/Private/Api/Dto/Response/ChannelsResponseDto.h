// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelConfigDto.h"
#include "ChannelMemberDto.h"
#include "ChannelStateResponseFieldsDto.h"
#include "CoreMinimal.h"
#include "UserDto.h"

#include "ChannelsResponseDto.generated.h"

/**
 * Represents channel in chat
 * #/components/schemas/ChannelsResponse
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
