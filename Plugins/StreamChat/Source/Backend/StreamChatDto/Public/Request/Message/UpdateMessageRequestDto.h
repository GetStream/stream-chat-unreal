// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageRequestDto.h"

#include "UpdateMessageRequestDto.generated.h"

/**
 * @brief #/components/schemas/UpdateMessageRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateMessageRequestDto
{
    GENERATED_BODY()

    /**
     * Message object
     */
    UPROPERTY()
    FMessageRequestDto Message;
};
