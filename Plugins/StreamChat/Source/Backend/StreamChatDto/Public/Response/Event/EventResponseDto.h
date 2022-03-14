// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EventDto.h"
#include "Response/ResponseDto.h"

#include "EventResponseDto.generated.h"

/**
 * @brief #/components/schemas/EventResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FEventResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FEventDto Event;
};
