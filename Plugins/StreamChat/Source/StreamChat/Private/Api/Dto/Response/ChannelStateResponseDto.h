// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelResponseDto.h"
#include "ChannelStateResponseFieldsDto.h"
#include "CoreMinimal.h"
#include "MessageDto.h"

#include "ChannelStateResponseDto.generated.h"

/**
 * #/components/schemas/ChannelStateResponse
 */
USTRUCT()
struct FChannelStateResponseDto : public FChannelStateResponseFieldsDto
{
    GENERATED_BODY()

    /// Duration of the request in human readable format
    UPROPERTY()
    FString Duration;
};
