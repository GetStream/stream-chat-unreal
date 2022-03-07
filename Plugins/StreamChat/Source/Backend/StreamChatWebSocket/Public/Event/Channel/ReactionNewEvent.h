// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionNewEvent.generated.h"

/**
 * @brief A new reaction was created
 * @see https://getstream.io/chat/docs/events/#reaction.new
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FReactionNewEvent : public FReactionEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.new");
};
