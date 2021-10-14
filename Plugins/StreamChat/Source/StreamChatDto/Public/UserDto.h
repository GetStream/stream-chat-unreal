// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UserDto.generated.h"

/**
 * #/components/schemas/UserObject
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FUserDto
{
    GENERATED_BODY()

    UPROPERTY()
    FString Id;
};
