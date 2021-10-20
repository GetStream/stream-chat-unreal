// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReactionRequestDto.h"

#include "SendReactionRequestDto.generated.h"

/**
 * Contains all information needed to send new message
 */
USTRUCT()
struct FSendReactionRequestDto
{
    GENERATED_BODY()

    /// Whether to replace all existing user reactions
    UPROPERTY()
    bool bEnforceUnique;

    UPROPERTY()
    FReactionRequestDto Reaction;

    /// Skips any mobile push notifications
    UPROPERTY()
    bool bSkipPush;
};
