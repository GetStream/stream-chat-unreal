// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
