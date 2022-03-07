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
    GENERATED_BODY()

    inline static FName StaticType = TEXT("connection.changed");

    UPROPERTY()
    bool bOnline = false;
};
