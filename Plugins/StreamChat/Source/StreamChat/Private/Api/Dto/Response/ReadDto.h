// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserDto.h"

#include "ReadDto.generated.h"

/**
 * #/components/schemas/Read
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FReadDto
{
    GENERATED_BODY()

    UPROPERTY()
    FDateTime LastRead;

    UPROPERTY()
    uint32 UnreadMessages;

    UPROPERTY()
    FUserDto User;
};
