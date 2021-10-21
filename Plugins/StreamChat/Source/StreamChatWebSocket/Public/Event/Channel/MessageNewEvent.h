#pragma once

#include "CoreMinimal.h"
#include "Event/Channel/MessageUpdatedEvent.h"

#include "MessageNewEvent.generated.h"

/**
 * #/components/schemas/MessageNewEvent
 */
USTRUCT()
struct FMessageNewEvent : public FMessageUpdatedEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.new");

    // TODO everything else
};
