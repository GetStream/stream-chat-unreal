// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Request/SortParamRequestDto.h"

#include "QueryThreadsRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryThreadsRequest
 * @see https://getstream.io/chat/docs/rest/#product:chat-threads-querythreads
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryThreadsRequestDto
{
    GENERATED_BODY()

    /// Conditions the returned threads must match
    UPROPERTY()
    FJsonObjectWrapper Filter;

    /// Number of threads to return (max 100)
    UPROPERTY()
    uint32 Limit = TNumericLimits<uint32>::Max();

    /// How many latest replies to include per thread (max 10)
    UPROPERTY()
    uint32 ReplyLimit = TNumericLimits<uint32>::Max();

    /// How many participants to include per thread (max 100)
    UPROPERTY()
    uint32 ParticipantLimit = TNumericLimits<uint32>::Max();

    /// How many members of the thread's channel to include per thread (max 100)
    UPROPERTY()
    uint32 MemberLimit = TNumericLimits<uint32>::Max();

    /// Value from a previous response, to paginate forwards
    UPROPERTY()
    FString Next;

    /// Value from a previous response, to paginate backwards
    UPROPERTY()
    FString Prev;

    /// List of sort parameters (max 5)
    UPROPERTY()
    TArray<FSortParamRequestDto> Sort;

    /// Start watching the channels the returned threads belong to
    UPROPERTY()
    bool bWatch = false;
};
