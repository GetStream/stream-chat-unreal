// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * @brief Additional channel actions to perform when requesting channel data from API
 * @ingroup StreamChatApi
 */
enum class EChannelFlags : uint8
{
    /// Do nothing
    None = 0,

    /// Refresh channel state
    State = 1 << 0,

    /// Start watching the channel
    Watch = 1 << 1,

    /// Fetch user presence info
    Presence = 1 << 2,
};
ENUM_CLASS_FLAGS(EChannelFlags);
