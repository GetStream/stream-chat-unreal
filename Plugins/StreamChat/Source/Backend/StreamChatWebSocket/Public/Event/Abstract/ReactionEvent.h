// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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

    /// The reaction the event is about.
    /// The message it belongs to is carried in full by FMessageEvent::Message, with its reaction
    /// counts and scores already updated to account for this event.
    UPROPERTY()
    FReactionDto Reaction;
};
