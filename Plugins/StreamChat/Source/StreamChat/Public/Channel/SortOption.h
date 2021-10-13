#pragma once

#include "CoreMinimal.h"

#include "SortOption.generated.h"

struct FSortOptionDto;

USTRUCT()
struct FSortOption
{
    GENERATED_BODY()

    explicit operator FSortOptionDto() const;
};
