// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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

    /// How many users were watching the channel when the message arrived.
    /// Left at TNumericLimits<uint32>::Max() if the API didn't send a count.
    UPROPERTY()
    uint32 WatcherCount = TNumericLimits<uint32>::Max();

    /// Users who have replied in the thread. Only sent when the message is a thread reply.
    UPROPERTY()
    TArray<FUserObjectDto> ThreadParticipants;
};
