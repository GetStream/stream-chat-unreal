#pragma once

#include "CoreMinimal.h"
#include "Dto/Request/SortOptionDto.h"

#include "SortOption.generated.h"

USTRUCT()
struct FSortOption
{
    GENERATED_BODY()

    explicit operator FSortOptionDto() const
    {
        return {};
    }
};
