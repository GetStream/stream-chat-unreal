// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Request/SortParamRequestDto.h"

#include "QueryChannelsRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryChannelsRequest
 * @see https://getstream.io/chat/docs/rest/#channels-querychannels-request
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryChannelsRequestDto
{
    GENERATED_BODY()

    /// The number of channels to return (max is 30)
    UPROPERTY()
    uint32 Limit = 10;

    /// The offset (max is 1000)
    UPROPERTY()
    uint32 Offset = 0;

    UPROPERTY()
    uint32 MessageLimit = 25;

    /// How many members should be included for each channel (Max 100)
    UPROPERTY()
    uint32 MemberLimit = 100;

    /// WebSocket connection ID to interact with
    UPROPERTY()
    FString ConnectionId;

    UPROPERTY()
    FJsonObjectWrapper FilterConditions;

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
