// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ChannelFlags.generated.h"

/**
 * @brief Additional channel actions to perform when requesting channel data from API
 * @ingroup StreamChatApi
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EChannelFlags : uint8
{
    /// Do nothing
    None = 0,

    /// Return channel state in response
    State = 1 << 0,

    /// Start watching the channel (receive events regarding this channel via WebSocket)
    Watch = 1 << 1,

    /// Return user presence info in response
    Presence = 1 << 2,
};
ENUM_CLASS_FLAGS(EChannelFlags);
