// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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

    inline static FName StaticType = TEXT("message.deleted");

    UPROPERTY()
    bool bHardDelete = false;

    // TODO everything else
};
