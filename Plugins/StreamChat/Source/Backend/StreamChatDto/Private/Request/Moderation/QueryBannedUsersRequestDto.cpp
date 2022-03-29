// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/Moderation/QueryBannedUsersRequestDto.h"

#include "Request/SortParamRequestDto.h"

FQueryBannedUsersRequestDto::FQueryBannedUsersRequestDto(const TOptional<uint32>& Limit, const TOptional<uint32>& Offset, const FJsonObjectWrapper& Filter)
    : FPaginatedRequest{Limit, Offset, Filter}
{
}

void FQueryBannedUsersRequestDto::SetCreatedAtAfter(const FDateTime& Value)
{
    Pagination.Set(TEXT("created_at_after"), Value);
}

void FQueryBannedUsersRequestDto::SetCreatedAtAfterOrEqual(const FDateTime& Value)
{
    Pagination.Set(TEXT("created_at_after_or_equal"), Value);
}

void FQueryBannedUsersRequestDto::SetCreatedAtBefore(const FDateTime& Value)
{
    Pagination.Set(TEXT("created_at_before"), Value);
}

void FQueryBannedUsersRequestDto::SetCreatedAtBeforeOrEqual(const FDateTime& Value)
{
    Pagination.Set(TEXT("created_at_before_or_equal"), Value);
}

void FQueryBannedUsersRequestDto::SetSort(const TArray<FSortParamRequestDto>& Value)
{
    if (Value.Num() > 0)
    {
        Pagination.SetArray(TEXT("sort"), Value);
    }
}
