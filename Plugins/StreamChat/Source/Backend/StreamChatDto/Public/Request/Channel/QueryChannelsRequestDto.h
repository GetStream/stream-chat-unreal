// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
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
struct STREAMCHATDTO_API FQueryChannelsRequestDto
{
    GENERATED_BODY()

    /// Number of channels to limit
    void SetLimit(int32);

    /// Number of members to limit
    void SetMemberLimit(int32);

    /// Number of messages to limit
    void SetMessageLimit(int32);

    /// Channel pagination offset
    void SetOffset(int32);

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

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
