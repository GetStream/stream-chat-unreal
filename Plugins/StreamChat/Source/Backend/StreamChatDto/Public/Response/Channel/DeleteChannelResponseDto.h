// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelResponseDto.h"
#include "CoreMinimal.h"
#include "Response/ResponseDto.h"

#include "DeleteChannelResponseDto.generated.h"

/**
 * @brief #/components/schemas/DeleteChannelResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FDeleteChannelResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FChannelResponseDto Channel;
};
