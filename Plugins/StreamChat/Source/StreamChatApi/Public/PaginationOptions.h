#pragma once

#include "CoreMinimal.h"

struct STREAMCHATAPI_API FPaginationOptions
{
    /// The number of channels to return (max is 30)
    TOptional<uint32> Limit;
    /// The offset (max is 1000)
    TOptional<uint32> Offset;
};
