#pragma once

#include "CoreMinimal.h"
#include "MessageUpdatedEvent.h"

#include "MessageDeletedEvent.generated.h"

/**
 * #/components/schemas/MessageDeletedEvent
 */
USTRUCT()
struct FMessageDeletedEvent : public FMessageUpdatedEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.deleted");

    UPROPERTY()
    bool bHardDelete;

    // TODO everything else
};
