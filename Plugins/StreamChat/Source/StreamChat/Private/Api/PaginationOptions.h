#pragma once

#include "CoreMinimal.h"

struct FPaginationOptions
{
    uint32 Limit;
    TOptional<uint32> Offset;
};
