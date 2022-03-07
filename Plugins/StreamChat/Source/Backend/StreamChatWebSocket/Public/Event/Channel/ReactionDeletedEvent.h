// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionDeletedEvent.generated.h"

/**
 * @brief A reaction was deleted
 * @see https://getstream.io/chat/docs/events/#reaction.deleted
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FReactionDeletedEvent : public FReactionEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("reaction.deleted")};
        return Type;
    }
};
