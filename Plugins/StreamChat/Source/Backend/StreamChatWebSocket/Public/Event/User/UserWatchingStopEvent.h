// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "UserWatchingStopEvent.generated.h"

/**
 * @brief #/components/schemas/UserWatchingStopEvent
 * @see https://getstream.io/chat/docs/events/#user.watching.stop
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FUserWatchingStopEvent : public FChannelEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("user.watching.stop")};
        return Type;
    }

    UPROPERTY()
    uint32 WatcherCount = TNumericLimits<uint32>::Max();

    UPROPERTY()
    FUserObjectDto User;
};
