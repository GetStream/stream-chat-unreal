// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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

    /// Whether the message was removed outright, rather than being marked as deleted.
    /// A hard deleted message never comes back from the API again.
    UPROPERTY()
    bool bHardDelete = false;

    /// Users who have replied in the thread. Only sent when the deleted message was a thread reply.
    UPROPERTY()
    TArray<FUserObjectDto> ThreadParticipants;
};
