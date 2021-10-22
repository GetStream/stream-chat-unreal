#pragma once

#include "CoreMinimal.h"

#include "SortOption.generated.h"

struct FSortOptionDto;

USTRUCT(BlueprintType)
struct FSortOption
{
    GENERATED_BODY()

    explicit operator FSortOptionDto() const;
};
