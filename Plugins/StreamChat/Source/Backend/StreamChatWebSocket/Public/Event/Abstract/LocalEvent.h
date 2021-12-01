// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "LocalEvent.generated.h"

/**
 * @brief Event not originating from the API
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FLocalEvent
{
    GENERATED_BODY()

    /// Event type
    UPROPERTY()
    FName Type;
};
