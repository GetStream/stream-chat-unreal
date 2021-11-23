// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionNewEvent.generated.h"

/**
 * #/components/schemas/ReactionNewEvent
 */
USTRUCT()
struct FReactionNewEvent : public FReactionEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.new");
};
