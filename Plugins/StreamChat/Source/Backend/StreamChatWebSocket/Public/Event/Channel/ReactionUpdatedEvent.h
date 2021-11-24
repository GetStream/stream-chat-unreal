// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionUpdatedEvent.generated.h"

/**
 * @brief #/components/schemas/ReactionUpdatedEvent
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FReactionUpdatedEvent : public FReactionEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.updated");
};
