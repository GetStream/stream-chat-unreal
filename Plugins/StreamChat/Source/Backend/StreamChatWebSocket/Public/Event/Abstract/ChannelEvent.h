// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChatEvent.h"
#include "CoreMinimal.h"

#include "ChannelEvent.generated.h"

/**
 * @brief Represents an event that happened in a Stream channel
 * @see https://getstream.io/chat/docs/other-rest/event_object/#event-object
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FChannelEvent : public FChatEvent
{
    GENERATED_BODY()

    UPROPERTY()
    FString ChannelId;

    UPROPERTY()
    FString ChannelType;

    /// Channel CID (<type>:<id>)
    UPROPERTY()
    FString Cid;
};
