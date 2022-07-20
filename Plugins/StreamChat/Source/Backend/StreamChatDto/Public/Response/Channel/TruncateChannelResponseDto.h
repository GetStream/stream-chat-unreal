// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelResponseDto.h"
#include "CoreMinimal.h"
#include "Response/Message/MessageDto.h"
#include "Response/ResponseDto.h"

#include "TruncateChannelResponseDto.generated.h"

/**
 * @brief #/components/schemas/TruncateChannelResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FTruncateChannelResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FChannelResponseDto Channel;

    UPROPERTY()
    FMessageDto Message;
};
