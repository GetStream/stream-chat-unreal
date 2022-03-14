// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Request/PaginatedRequest.h"
#include "Request/SortParamRequestDto.h"

#include "QueryChannelsRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryChannelsRequest
 * @see https://getstream.io/chat/docs/rest/#channels-querychannels-request
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryChannelsRequestDto : public FPaginatedRequest
{
    GENERATED_BODY()

    FQueryChannelsRequestDto() = default;
    explicit FQueryChannelsRequestDto(
        const uint32 MessageLimit,
        const uint32 MemberLimit,
        const FString& ConnectionId,
        const bool bPresence,
        const TArray<FSortParamRequestDto>& Sort,
        const bool bState,
        const bool bWatch,
        const TOptional<uint32>& Limit,
        const TOptional<uint32>& Offset,
        const FJsonObjectWrapper& Filter);

    UPROPERTY()
    uint32 MessageLimit = 25;

    /// How many members should be included for each channel (Max 100)
    UPROPERTY()
    uint32 MemberLimit = 100;

    /// WebSocket connection ID to interact with
    UPROPERTY()
    FString ConnectionId;

    /// Fetch user presence info
    UPROPERTY()
    bool bPresence = false;

    /// List of sort parameters
    UPROPERTY()
    TArray<FSortParamRequestDto> Sort;

    /// Refresh channel state
    UPROPERTY()
    bool bState = true;

    /// Start watching the channel
    UPROPERTY()
    bool bWatch = true;
};
