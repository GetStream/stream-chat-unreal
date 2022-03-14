// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Event/EventDto.h"
#include "Response/ResponseDto.h"

#include "MarkReadResponseDto.generated.h"

/**
 * @brief #/components/schemas/MarkReadResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMarkReadResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// Mark read event
    UPROPERTY()
    FEventDto Event;
};
