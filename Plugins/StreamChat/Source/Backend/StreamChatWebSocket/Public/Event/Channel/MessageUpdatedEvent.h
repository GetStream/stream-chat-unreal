// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "MessageUpdatedEvent.generated.h"

/**
 * @brief A message's content was updated
 * @see https://getstream.io/chat/docs/events/#message.updated
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FMessageUpdatedEvent : public FMessageEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("message.updated")};
        return Type;
    }

    // TODO everything else
};
