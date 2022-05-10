// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Request/SortParamRequestDto.h"
#include "Response/Channel/ChannelMemberDto.h"

#include "QueryMembersRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryMembersRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FQueryMembersRequestDto
{
    GENERATED_BODY()

    /// Channel type to query members of
    UPROPERTY()
    FString Type;

    /// Channel ID to query members of
    UPROPERTY()
    FString Id;

    /// List of members to search in distinct channels
    UPROPERTY()
    TArray<FChannelMemberDto> Members;

    /// Filter to apply to members
    UPROPERTY()
    FJsonObjectWrapper FilterConditions;

    /// List of sort parameters
    UPROPERTY()
    TArray<FSortParamRequestDto> Sort;

    UPROPERTY()
    uint32 Limit = TNumericLimits<uint32>::Max();

    UPROPERTY()
    uint32 Offset = TNumericLimits<uint32>::Max();

    UPROPERTY()
    FString UserIdGt;

    UPROPERTY()
    FString UserIdGte;

    UPROPERTY()
    FString UserIdLt;

    UPROPERTY()
    FString UserIdLte;

    UPROPERTY()
    FDateTime CreatedAtAfter = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtAfterOrEqual = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtBefore = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtBeforeOrEqual = FDateTime{0};
};
