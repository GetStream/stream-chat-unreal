// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CommandDto.generated.h"

/**
 * @brief #/components/schemas/Command
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FCommandDto
{
    GENERATED_BODY()

    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FString Description;

    UPROPERTY()
    FString Args;

    UPROPERTY()
    FString Set;
};
