#pragma once

#include "CoreMinimal.h"

#include "SortOptionDto.generated.h"

struct FSortOption;

USTRUCT()
struct FSortOptionDto
{
    GENERATED_BODY()

    FSortOptionDto() = default;
    explicit FSortOptionDto(const FSortOption&);
};
