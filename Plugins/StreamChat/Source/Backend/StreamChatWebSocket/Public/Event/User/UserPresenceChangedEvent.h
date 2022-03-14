// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChatEvent.h"
#include "UserObjectDto.h"

#include "UserPresenceChangedEvent.generated.h"

/**
 * @brief #/components/schemas/UserPresenceChangedEvent
 * @see https://getstream.io/chat/docs/events/#user.presence.changed
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FUserPresenceChangedEvent : public FChatEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("user.presence.changed")};
        return Type;
    }

    UPROPERTY()
    FUserObjectDto User;
};
