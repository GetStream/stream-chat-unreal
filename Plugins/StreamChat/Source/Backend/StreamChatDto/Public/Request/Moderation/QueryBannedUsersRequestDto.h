// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Request/PaginatedRequest.h"

#include "QueryBannedUsersRequestDto.generated.h"

struct FSortParamRequestDto;
/**
 * @brief #/components/schemas/QueryBannedUsersRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryBannedUsersRequestDto : public FPaginatedRequest
{
    GENERATED_BODY()

    FQueryBannedUsersRequestDto() = default;

    FQueryBannedUsersRequestDto(const TOptional<uint32>& Limit, const TOptional<uint32>& Offset, const FJsonObjectWrapper& Filter);

    void SetCreatedAtAfter(const FDateTime&);

    void SetCreatedAtAfterOrEqual(const FDateTime&);

    void SetCreatedAtBefore(const FDateTime&);

    void SetCreatedAtBeforeOrEqual(const FDateTime&);

    /// Sort parameters. Cannot be used with non-zero offset
    void SetSort(const TArray<FSortParamRequestDto>&);
};
