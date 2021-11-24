// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * @brief Options for pagination of channels
 * @ingroup StreamChatApi
 */
struct STREAMCHATAPI_API FPaginationOptions
{
    /// The number of channels to return (max is 30)
    TOptional<uint32> Limit;
    /// The offset (max is 1000)
    TOptional<uint32> Offset;
};
