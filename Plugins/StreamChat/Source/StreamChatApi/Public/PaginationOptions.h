#pragma once

#include "CoreMinimal.h"

struct STREAMCHATAPI_API FPaginationOptions
{
    TOptional<uint32> Limit;
    TOptional<uint32> Offset;
};
