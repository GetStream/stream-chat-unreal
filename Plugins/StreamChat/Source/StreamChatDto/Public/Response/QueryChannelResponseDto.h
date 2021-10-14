// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStateResponseFieldsDto.h"
#include "CoreMinimal.h"

#include "QueryChannelResponseDto.generated.h"

/**
 *
 */
USTRUCT()
struct FQueryChannelResponseDto
{
    GENERATED_BODY()

    /// List of channels
    UPROPERTY()
    TArray<FChannelStateResponseFieldsDto> Channels;

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;
};
