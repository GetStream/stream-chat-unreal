// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "MessageDeletedEvent.generated.h"

/**
 * @brief A message was deleted
 * @see https://getstream.io/chat/docs/events/#message.deleted
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FMessageDeletedEvent : public FMessageEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("message.deleted")};
        return Type;
    }

    UPROPERTY()
    bool bHardDelete = false;

    // TODO everything else
};
