// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionDeletedEvent.generated.h"

/**
 * @brief #/components/schemas/ReactionDeletedEvent
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FReactionDeletedEvent : public FReactionEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("reaction.deleted");
};
