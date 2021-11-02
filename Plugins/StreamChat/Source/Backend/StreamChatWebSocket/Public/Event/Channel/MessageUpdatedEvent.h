#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "MessageUpdatedEvent.generated.h"

/**
 * #/components/schemas/MessageUpdatedEvent
 */
USTRUCT()
struct FMessageUpdatedEvent : public FMessageEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.updated");

    // TODO everything else
};
