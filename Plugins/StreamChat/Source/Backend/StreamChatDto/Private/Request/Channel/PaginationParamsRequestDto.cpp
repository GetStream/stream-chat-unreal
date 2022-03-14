// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/PaginationParamsRequestDto.h"

void FPaginationParamsRequestDto::SetIdGt(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_gt"), Value);
}

void FPaginationParamsRequestDto::SetIdGte(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_gte"), Value);
}

void FPaginationParamsRequestDto::SetIdLt(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_lt"), Value);
}

void FPaginationParamsRequestDto::SetIdLte(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_lte"), Value);
}
