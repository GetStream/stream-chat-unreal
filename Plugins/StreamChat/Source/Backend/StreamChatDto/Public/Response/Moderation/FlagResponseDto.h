// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlagDto.h"
#include "Response/ResponseDto.h"

#include "FlagResponseDto.generated.h"

/**
 * @brief #/components/schemas/FlagResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FFlagResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FFlagDto Flag;
};
