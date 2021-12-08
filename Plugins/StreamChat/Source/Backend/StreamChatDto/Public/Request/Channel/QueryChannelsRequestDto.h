// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "JsonObjectWrapper.h"
#include "SortParamRequestDto.h"

#include "QueryChannelsRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryChannelsRequest
 * @see https://getstream.io/chat/docs/rest/#channels-querychannels-request
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FQueryChannelsRequestDto
{
    GENERATED_BODY()

    STREAMCHATDTO_API static void SerializeExtra(const FQueryChannelsRequestDto&, FJsonObject&);

    /// WebSocket connection ID to interact with
    UPROPERTY()
    FString ConnectionId;

    UPROPERTY()
    FJsonObjectWrapper FilterConditions;

    /// Number of channels to limit
    TOptional<uint32> Limit;

    /// Number of members to limit
    TOptional<uint32> MemberLimit;

    /// Number of messages to limit
    TOptional<uint32> MessageLimit;

    /// Channel pagination offset
    TOptional<uint32> Offset;

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
