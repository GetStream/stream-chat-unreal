// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStateResponseFieldsDto.h"
#include "CoreMinimal.h"
#include "Response/ResponseDto.h"

#include "ChannelsResponseDto.generated.h"

/**
 * @brief #/components/schemas/ChannelsResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FChannelsResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// List of channels
    UPROPERTY()
    TArray<FChannelStateResponseFieldsDto> Channels;
};
