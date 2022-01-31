// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Request/User/QueryUsersRequestDto.h"

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
