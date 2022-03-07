// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/LocalEvent.h"

#include "ConnectionRecoveredEvent.generated.h"

/**
 * @brief Sent when the connection is recovered
 * @note This event is not emitted from the API, but is generated locally
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FConnectionRecoveredEvent : public FLocalEvent
{
    GENERATED_BODY()

    explicit FConnectionRecoveredEvent() : FLocalEvent{StaticType()}
    {
        ;
    };

    static FName StaticType()
    {
        static const FName Type{TEXT("connection.recovered")};
        return Type;
    }
};
