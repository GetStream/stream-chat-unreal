#pragma once

#include "CoreMinimal.h"

#include "FilterDto.generated.h"

struct FFilter;

USTRUCT()
struct FFilterDto
{
    FFilterDto() = default;
    explicit FFilterDto(const FFilter&);
    GENERATED_BODY()
};
