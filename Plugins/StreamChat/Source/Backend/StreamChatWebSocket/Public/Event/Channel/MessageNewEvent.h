// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "MessageNewEvent.generated.h"

/**
 * @brief A new message was created
 * @see https://getstream.io/chat/docs/events/#message.new
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FMessageNewEvent : public FMessageEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("message.new")};
        return Type;
    }

    // TODO everything else
};
