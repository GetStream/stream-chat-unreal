// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/LocalEvent.h"

#include "ConnectionChangedEvent.generated.h"

/**
 * @brief Sent when the state of the connection changed
 * @note This event is not emitted from the API, but is generated locally
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FConnectionChangedEvent : public FLocalEvent
{
    explicit FConnectionChangedEvent() = default;
    explicit FConnectionChangedEvent(const bool bOnline) : FLocalEvent{StaticType()}, bOnline(bOnline)
    {
    }

    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("connection.changed")};
        return Type;
    }

    UPROPERTY()
    bool bOnline = false;
};
