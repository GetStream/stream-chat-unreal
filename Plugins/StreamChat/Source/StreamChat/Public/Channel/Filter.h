#pragma once

#include "CoreMinimal.h"
#include "Dto/Request/FilterDto.h"

#include "Filter.generated.h"

USTRUCT()
struct FFilter
{
    GENERATED_BODY()

    explicit operator FFilterDto() const
    {
        return {};
    }
};
