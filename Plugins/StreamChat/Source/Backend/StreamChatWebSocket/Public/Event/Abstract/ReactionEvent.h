// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageEvent.h"

#include "ReactionEvent.generated.h"

/**
 * @brief #/components/schemas/ReactionNewEvent
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FReactionEvent : public FMessageEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.new");

    UPROPERTY()
    FReactionDto Reaction;

    // TODO everything else
};
