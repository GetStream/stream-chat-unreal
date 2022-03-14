// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/User/QueryUsersRequestDto.h"

FQueryUsersRequestDto::FQueryUsersRequestDto(
    const FString& ConnectionId,
    const bool bPresence,
    const TArray<FSortParamRequestDto>& Sort,
    const TOptional<uint32>& Limit,
    const TOptional<uint32>& Offset,
    const FJsonObjectWrapper& Filter)
    : FPaginatedRequest{Limit, Offset, Filter}, ConnectionId{ConnectionId}, bPresence{bPresence}, Sort{Sort}
{
}

void FQueryUsersRequestDto::SetIdGt(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_gt"), Value);
}

void FQueryUsersRequestDto::SetIdGte(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_gte"), Value);
}

void FQueryUsersRequestDto::SetIdLt(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_lt"), Value);
}

void FQueryUsersRequestDto::SetIdLte(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_lte"), Value);
}
