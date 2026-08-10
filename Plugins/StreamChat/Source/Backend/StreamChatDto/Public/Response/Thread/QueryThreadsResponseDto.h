// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/ResponseDto.h"
#include "Response/Thread/ThreadStateResponseDto.h"

#include "QueryThreadsResponseDto.generated.h"

/**
 * @brief #/components/schemas/QueryThreadsResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FQueryThreadsResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// The matching threads, enriched with their latest replies
    UPROPERTY()
    TArray<FThreadStateResponseDto> Threads;

    /// Value to pass to the next query in order to paginate forwards
    UPROPERTY()
    FString Next;

    /// Value to pass to the next query in order to paginate backwards
    UPROPERTY()
    FString Prev;
};
