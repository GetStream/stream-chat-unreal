// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "UserUnbannedEvent.generated.h"

/**
 * @brief #/components/schemas/UserUnbannedEvent
 * @see https://getstream.io/chat/docs/events/#user.unbanned
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FUserUnbannedEvent : public FChannelEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("user.unbanned")};
        return Type;
    }

    UPROPERTY()
    bool bShadow = false;
    UPROPERTY()
    FUserObjectDto User;
};
