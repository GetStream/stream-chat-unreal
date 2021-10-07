// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UserDto.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUserDto
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Id;
};
