// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "UserWatchingStartEvent.generated.h"

/**
 * @brief #/components/schemas/UserWatchingStartEvent
 * @see https://getstream.io/chat/docs/events/#user.watching.start
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FUserWatchingStartEvent : public FChannelEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("user.watching.start")};
        return Type;
    }

    UPROPERTY()
    FString Team;

    UPROPERTY()
    uint32 WatcherCount = TNumericLimits<uint32>::Max();

    UPROPERTY()
    FUserObjectDto User;
};
