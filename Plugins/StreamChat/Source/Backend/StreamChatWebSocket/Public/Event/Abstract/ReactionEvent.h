// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageEvent.h"

#include "ReactionEvent.generated.h"

/**
 * @brief A WebSocket event related to a reaction
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FReactionEvent : public FMessageEvent
{
    GENERATED_BODY()

    UPROPERTY()
    FReactionDto Reaction;

    // TODO everything else
};
