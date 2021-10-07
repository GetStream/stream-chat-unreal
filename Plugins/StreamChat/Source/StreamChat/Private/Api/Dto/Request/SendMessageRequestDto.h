// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageRequestDto.h"

#include "SendMessageRequestDto.generated.h"

/**
 * Contains all information needed to send new message
 */
USTRUCT()
struct FSendMessageRequestDto
{
    GENERATED_BODY()

    /**
     * Message object
     */
    UPROPERTY()
    FMessageRequestDto Message;

    /**
     * Disables all push notifications for this message
     */
    UPROPERTY()
    bool bSkipPush;
};
