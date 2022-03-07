// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ReactionEvent.h"

#include "ReactionUpdatedEvent.generated.h"

/**
 * @brief A reaction was updated
 * @see https://getstream.io/chat/docs/events/#reaction.updated
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FReactionUpdatedEvent : public FReactionEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("reaction.updated")};
        return Type;
    }
};
