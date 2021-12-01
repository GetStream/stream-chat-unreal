// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStateResponseFieldsDto.h"
#include "CoreMinimal.h"

#include "ChannelStateResponseDto.generated.h"

/**
 * @brief #/components/schemas/ChannelStateResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FChannelStateResponseDto : public FChannelStateResponseFieldsDto
{
    GENERATED_BODY()

    /// Duration of the request in human readable format
    UPROPERTY()
    FString Duration;
};
