// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "Response/Message/MessageDto.h"

#include "MessageEvent.generated.h"

/**
 * @brief A WebSocket event related to a message
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FMessageEvent : public FChannelEvent
{
    GENERATED_BODY()

    UPROPERTY()
    FMessageDto Message;
};
