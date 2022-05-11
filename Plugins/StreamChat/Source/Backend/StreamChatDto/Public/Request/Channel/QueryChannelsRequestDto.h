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

    UPROPERTY()
    uint32 MessageLimit = TNumericLimits<uint32>::Max();

    /// How many members should be included for each channel (Max 100)
    UPROPERTY()
    uint32 MemberLimit = TNumericLimits<uint32>::Max();

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
