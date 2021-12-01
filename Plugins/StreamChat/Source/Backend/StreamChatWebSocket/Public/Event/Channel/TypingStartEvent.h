// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "TypingStartEvent.generated.h"

/**
 * @brief A user started typing
 * @see https://getstream.io/chat/docs/events/#typing.start
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FTypingStartEvent : public FChannelEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("typing.start");

    UPROPERTY()
    FString ParentId;

    UPROPERTY()
    FUserObjectDto User;
};
