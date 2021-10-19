#pragma once

#include "ChatEvent.h"
#include "CoreMinimal.h"
#include "Response/MessageDto.h"

#include "ReactionEvent.generated.h"

/**
 * #/components/schemas/ReactionNewEvent
 */
USTRUCT()
struct FReactionEvent : public FChatEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.new");

    UPROPERTY()
    FMessageDto Message;

    UPROPERTY()
    FReactionDto Reaction;

    // TODO everything else
};
