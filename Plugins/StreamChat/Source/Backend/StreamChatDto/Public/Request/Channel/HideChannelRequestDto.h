// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HideChannelRequestDto.generated.h"

/**
 * @brief #/components/schemas/HideChannelRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FHideChannelRequestDto
{
    GENERATED_BODY()

    /// Whether to clear message history of the channel or not
    UPROPERTY()
    bool bClearHistory = false;
};
