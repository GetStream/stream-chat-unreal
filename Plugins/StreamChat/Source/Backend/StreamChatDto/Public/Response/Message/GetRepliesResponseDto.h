// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Message/MessageDto.h"
#include "Response/ResponseDto.h"

#include "GetRepliesResponseDto.generated.h"

/**
 * @brief #/components/schemas/GetRepliesResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FGetRepliesResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// The replies of the thread, oldest first
    UPROPERTY()
    TArray<FMessageDto> Messages;
};
