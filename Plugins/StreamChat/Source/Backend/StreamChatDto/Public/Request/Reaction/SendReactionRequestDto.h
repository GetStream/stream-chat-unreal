// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReactionRequestDto.h"

#include "SendReactionRequestDto.generated.h"

/**
 * @brief #/components/schemas/SendReactionRequest
 * Contains all information needed to send new reaction
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FSendReactionRequestDto
{
    GENERATED_BODY()

    /// Whether to replace all existing user reactions
    UPROPERTY()
    bool bEnforceUnique = false;

    UPROPERTY()
    FReactionRequestDto Reaction;

    /// Skips any mobile push notifications
    UPROPERTY()
    bool bSkipPush = false;
};
