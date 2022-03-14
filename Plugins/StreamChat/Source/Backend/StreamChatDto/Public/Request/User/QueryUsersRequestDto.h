// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Request/SortParamRequestDto.h"

#include "QueryUsersRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryUsersRequest
 * @see https://getstream.io/chat/docs/rest/#channels-querychannels-request
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryUsersRequestDto
{
    GENERATED_BODY()

    /// Matches IDs that are greater than the specified ID
    void SetIdGt(const FString&);

    /// Matches IDs that are greater than or equal to the specified ID
    void SetIdGte(const FString&);

    /// Matches IDs that are less than the specified ID
    void SetIdLt(const FString&);

    /// Matches IDs that are less than or equal to the specified ID
    void SetIdLte(const FString&);

    /// WebSocket connection ID to interact with
    UPROPERTY()
    FString ConnectionId;

    /// Number of records to return
    UPROPERTY()
    uint32 Limit = 30;

    /// Number of records to offset
    /// Note - The offset limit is set to 1000.
    UPROPERTY()
    uint32 Offset = 0;

    UPROPERTY()
    FJsonObjectWrapper FilterConditions;

    /// Fetch user presence info
    /// You can subscribe to presence status of at most 30 users using this method.
    UPROPERTY()
    bool bPresence = true;

    /// List of sort parameters
    UPROPERTY()
    TArray<FSortParamRequestDto> Sort = {{TEXT("created_at"), -1}, {TEXT("id"), -1}};

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
