// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Request/PaginatedRequest.h"
#include "Request/SortParamRequestDto.h"

#include "QueryBannedUsersRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryBannedUsersRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryBannedUsersRequestDto : public FPaginatedRequest
{
    GENERATED_BODY()

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
