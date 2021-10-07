// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CommandDto.generated.h"

/**
 *
 */
USTRUCT()
struct FCommandDto
{
    GENERATED_BODY()

    UPROPERTY()
    FDateTime CreatedAt;

    UPROPERTY()
    FDateTime UpdatedAt;

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FString Description;

    UPROPERTY()
    FString Args;

    UPROPERTY()
    FString Set;
};
