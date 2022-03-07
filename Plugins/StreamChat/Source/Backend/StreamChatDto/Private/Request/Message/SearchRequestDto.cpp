// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/Message/SearchRequestDto.h"

#include "Dom/JsonObject.h"
#include "Request/SortParamRequestDto.h"

void FSearchRequestDto::SetQuery(const FString& Value)
{
    if (!Value.IsEmpty())
    {
        AdditionalFields.SetString(TEXT("query"), Value);
    }
}

void FSearchRequestDto::SetMessageFilter(const TSharedRef<FJsonObject>& Value)
{
    AdditionalFields.SetJsonValue(TEXT("message_filter_conditions"), MakeShared<FJsonValueObject>(Value));
}

void FSearchRequestDto::SetSort(const TArray<FSortParamRequestDto>& Value)
{
    if (Value.Num() > 0)
    {
        AdditionalFields.SetArray(TEXT("sort"), Value);
    }
}

void FSearchRequestDto::SetMessageLimit(const int32 Value)
{
    AdditionalFields.SetNumber(TEXT("limit"), Value);
}

void FSearchRequestDto::SetOffset(const int32 Value)
{
    AdditionalFields.SetNumber(TEXT("offset"), Value);
}

void FSearchRequestDto::SetNext(const FString& Value)
{
    AdditionalFields.SetString(TEXT("next"), Value);
}
