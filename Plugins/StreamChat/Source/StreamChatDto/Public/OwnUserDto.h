// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "OwnUserDto.generated.h"

/**
 * #/components/schemas/OwnUser
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FOwnUserDto
{
    GENERATED_BODY()

    UPROPERTY()
    FString Id;
};
