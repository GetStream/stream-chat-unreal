// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelResponseDto.h"
#include "CoreMinimal.h"
#include "Response/Message/MessageDto.h"
#include "Response/ResponseDto.h"

#include "UpdateChannelResponseDto.generated.h"

/**
 * @brief #/components/schemas/UpdateChannelResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateChannelResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FChannelResponseDto Channel;

    UPROPERTY()
    FMessageDto Message;

    UPROPERTY()
    TArray<FChannelMemberDto> Members;
};
