// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelResponseDto.h"
#include "CoreMinimal.h"
#include "Response/ResponseDto.h"

#include "UpdateChannelPartialResponseDto.generated.h"

/**
 * @brief #/components/schemas/UpdateChannelPartialResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateChannelPartialResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// Represents channel in chat
    UPROPERTY()
    FChannelResponseDto Channel;

    UPROPERTY()
    TArray<FChannelMemberDto> Members;
};
