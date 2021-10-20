#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionDeletedEvent.generated.h"

/**
 * #/components/schemas/ReactionDeletedEvent
 */
USTRUCT()
struct FReactionDeletedEvent : public FReactionEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.deleted");
};
