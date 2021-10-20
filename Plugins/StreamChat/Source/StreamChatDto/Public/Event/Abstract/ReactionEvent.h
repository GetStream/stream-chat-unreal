#pragma once

#include "ChannelEvent.h"
#include "CoreMinimal.h"
#include "Response/Message/MessageDto.h"

#include "ReactionEvent.generated.h"

/**
 * #/components/schemas/ReactionNewEvent
 */
USTRUCT()
struct FReactionEvent : public FChannelEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.new");

    UPROPERTY()
    FMessageDto Message;

    UPROPERTY()
    FReactionDto Reaction;

    // TODO everything else
};
