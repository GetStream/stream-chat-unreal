// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "Request/PaginatedRequest.h"
#include "Request/SortParamRequestDto.h"

#include "QueryUsersRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryUsersRequest
 * @see https://getstream.io/chat/docs/rest/#channels-querychannels-request
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryUsersRequestDto : public FPaginatedRequest
{
    GENERATED_BODY()

    /// Matches IDs that are greater than the specified ID
    UPROPERTY()
    FString IdGt;

    /// Matches IDs that are greater than or equal to the specified ID
    UPROPERTY()
    FString IdGte;

    /// Matches IDs that are less than the specified ID
    UPROPERTY()
    FString IdLt;

    /// Matches IDs that are less than or equal to the specified ID
    UPROPERTY()
    FString IdLte;

    /// WebSocket connection ID to interact with
    UPROPERTY()
    FString ConnectionId;

    /// Fetch user presence info
    /// You can subscribe to presence status of at most 30 users using this method.
    UPROPERTY()
    bool bPresence = true;

    /// List of sort parameters
    UPROPERTY()
    TArray<FSortParamRequestDto> Sort;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
