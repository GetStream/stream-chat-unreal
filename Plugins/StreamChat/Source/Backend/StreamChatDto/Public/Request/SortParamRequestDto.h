// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SortParamRequestDto.generated.h"

/**
 * @brief #/components/schemas/SortParamRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FSortParamRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    FString Field;

    UPROPERTY()
    int32 Direction = -1;
};
