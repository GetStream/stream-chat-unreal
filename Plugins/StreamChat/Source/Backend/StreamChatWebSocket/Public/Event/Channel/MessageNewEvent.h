#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "MessageNewEvent.generated.h"

/**
 * #/components/schemas/MessageNewEvent
 */
USTRUCT()
struct FMessageNewEvent : public FMessageEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.new");

    // TODO everything else
};
