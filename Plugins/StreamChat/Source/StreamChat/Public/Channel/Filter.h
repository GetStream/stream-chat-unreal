#pragma once

#include "CoreMinimal.h"

#include "Filter.generated.h"

struct FFilterDto;

USTRUCT()
struct FFilter
{
    GENERATED_BODY()

    explicit operator FFilterDto() const;
};
