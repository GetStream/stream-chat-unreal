// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageDto.h"
#include "Response/ResponseDto.h"

#include "MessageResponseDto.generated.h"

/**
 * @brief #/components/schemas/MessageResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMessageResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FMessageDto Message;
};
