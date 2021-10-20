#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChatEvent.h"
#include "Response/Message/MessageDto.h"

#include "MessageNewEvent.generated.h"

/**
 * #/components/schemas/MessageNewEvent
 */
USTRUCT()
struct FMessageNewEvent : public FChatEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.new");

    UPROPERTY()
    FMessageDto Message;

    // TODO everything else
};
