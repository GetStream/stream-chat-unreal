// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "TypingStopEvent.generated.h"

/**
 * @brief A user stoped typing
 * @see https://getstream.io/chat/docs/events/#typing.stop
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FTypingStopEvent : public FChannelEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("typing.stop");

    UPROPERTY()
    FString ParentId;

    UPROPERTY()
    FUserObjectDto User;
};
