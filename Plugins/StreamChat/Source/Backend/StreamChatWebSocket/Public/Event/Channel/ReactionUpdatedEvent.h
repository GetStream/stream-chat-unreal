// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionUpdatedEvent.generated.h"

/**
 * #/components/schemas/ReactionUpdatedEvent
 */
USTRUCT()
struct FReactionUpdatedEvent : public FReactionEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.updated");
};
