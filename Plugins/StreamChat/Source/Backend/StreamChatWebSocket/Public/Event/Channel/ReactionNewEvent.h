// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionNewEvent.generated.h"

/**
 * @brief #/components/schemas/ReactionNewEvent
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FReactionNewEvent : public FReactionEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.new");
};
