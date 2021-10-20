#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "Response/Message/MessageDto.h"

#include "MessageUpdatedEvent.generated.h"

/**
 * #/components/schemas/MessageUpdatedEvent
 */
USTRUCT()
struct FMessageUpdatedEvent : public FChannelEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.updated");

    UPROPERTY()
    FMessageDto Message;

    // TODO everything else
};
