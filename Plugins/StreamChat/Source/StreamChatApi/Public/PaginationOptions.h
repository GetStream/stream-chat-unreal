#pragma once

#include "CoreMinimal.h"

struct STREAMCHATAPI_API FPaginationOptions
{
    uint32 Limit;
    TOptional<uint32> Offset;
};
