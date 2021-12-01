// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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

    inline static FName StaticType = TEXT("message.updated");

    // TODO everything else
};
