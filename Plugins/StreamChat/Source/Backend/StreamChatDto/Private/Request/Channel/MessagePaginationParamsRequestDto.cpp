// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/MessagePaginationParamsRequestDto.h"

#include "StreamJsonField.h"

void FMessagePaginationParamsRequestDto::SetCreatedAtAfter(const FDateTime& Value)
{
    AdditionalFields.Set(TEXT("created_at_after"), Value);
}

void FMessagePaginationParamsRequestDto::SetCreatedAtAfterOrEqual(const FDateTime& Value)
{
    AdditionalFields.Set(TEXT("created_at_after_or_equal"), Value);
}

void FMessagePaginationParamsRequestDto::SetCreatedAtBefore(const FDateTime& Value)
{
    AdditionalFields.Set(TEXT("created_at_before"), Value);
}

void FMessagePaginationParamsRequestDto::SetCreatedAtBeforeOrEqual(const FDateTime& Value)
{
    AdditionalFields.Set(TEXT("created_at_before_or_equal"), Value);
}

void FMessagePaginationParamsRequestDto::SetIdGt(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_gt"), Value);
}

void FMessagePaginationParamsRequestDto::SetIdGte(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_gte"), Value);
}

void FMessagePaginationParamsRequestDto::SetIdLt(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_lt"), Value);
}

void FMessagePaginationParamsRequestDto::SetIdLte(const FString& Value)
{
    AdditionalFields.SetString(TEXT("id_lte"), Value);
}
