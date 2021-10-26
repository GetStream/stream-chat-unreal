#pragma once

#include "CoreMinimal.h"

#include "SortParamRequestDto.generated.h"

/**
 * #/components/schemas/SortParamRequest
 */
USTRUCT()
struct STREAMCHATDTO_API FSortParamRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    FString Field;

    UPROPERTY()
    int32 Direction;
};
