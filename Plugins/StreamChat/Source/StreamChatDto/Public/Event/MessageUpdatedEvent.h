#pragma once

#include "ChatEvent.h"
#include "CoreMinimal.h"
#include "Response/MessageDto.h"

#include "MessageUpdatedEvent.generated.h"

/**
 * #/components/schemas/MessageUpdatedEvent
 */
USTRUCT()
struct FMessageUpdatedEvent : public FChatEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.updated");

    UPROPERTY()
    FMessageDto Message;

    // TODO everything else
};
