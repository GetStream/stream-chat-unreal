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

    FQueryUsersRequestDto() = default;
    explicit FQueryUsersRequestDto(
        const FString& ConnectionId,
        const bool bPresence,
        const TArray<FSortParamRequestDto>& Sort,
        const TOptional<uint32>& Limit,
        const TOptional<uint32>& Offset,
        const FJsonObjectWrapper& Filter);

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
