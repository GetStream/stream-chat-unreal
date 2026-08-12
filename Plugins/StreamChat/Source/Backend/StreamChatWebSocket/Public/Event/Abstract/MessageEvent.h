// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "Response/Message/MessageDto.h"
#include "UserObjectDto.h"

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

    /// The user who caused the event, which is the message's author for everything but a deletion
    /// performed by somebody else.
    UPROPERTY()
    FUserObjectDto User;

    /// Team the channel belongs to. Only sent by multi-tenant apps.
    UPROPERTY()
    FString Team;
};
