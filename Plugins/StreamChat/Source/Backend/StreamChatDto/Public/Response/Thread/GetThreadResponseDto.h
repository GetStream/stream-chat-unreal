// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/ResponseDto.h"
#include "Response/Thread/ThreadStateResponseDto.h"

#include "GetThreadResponseDto.generated.h"

/**
 * @brief #/components/schemas/GetThreadResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FGetThreadResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// The requested thread, enriched with its latest replies
    UPROPERTY()
    FThreadStateResponseDto Thread;
};
