// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "MessageReadEvent.generated.h"

/**
 * @brief A new message was created
 * @see https://getstream.io/chat/docs/events/#message.read
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FMessageReadEvent : public FChannelEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("message.read")};
        return Type;
    }

    UPROPERTY()
    FUserObjectDto User;
};
