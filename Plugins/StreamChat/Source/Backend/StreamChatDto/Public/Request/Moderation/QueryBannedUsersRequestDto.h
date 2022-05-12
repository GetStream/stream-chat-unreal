// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Request/SortParamRequestDto.h"

#include "QueryBannedUsersRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryBannedUsersRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryBannedUsersRequestDto
{
    GENERATED_BODY()

    /// Number of items to return
    UPROPERTY()
    uint32 Limit = TNumericLimits<uint32>::Max();

    /// Item offset
    UPROPERTY()
    uint32 Offset = TNumericLimits<uint32>::Max();

    UPROPERTY()
    FJsonObjectWrapper FilterConditions;

    UPROPERTY()
    FDateTime CreatedAtAfter = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtAfterOrEqual = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtBefore = FDateTime{0};

    UPROPERTY()
    FDateTime CreatedAtBeforeOrEqual = FDateTime{0};

    /// Sort parameters. Cannot be used with non-zero offset
    UPROPERTY()
    TArray<FSortParamRequestDto> Sort;
};
