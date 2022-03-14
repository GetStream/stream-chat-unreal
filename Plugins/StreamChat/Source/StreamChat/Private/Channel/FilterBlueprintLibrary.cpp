// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/FilterBlueprintLibrary.h"

FFilter UFilterBlueprintLibrary::And(const TArray<FFilter>& Filters)
{
    return FFilter::And(Filters);
}

FFilter UFilterBlueprintLibrary::Or(const TArray<FFilter>& Filters)
{
    return FFilter::Or(Filters);
}

FFilter UFilterBlueprintLibrary::Nor(const TArray<FFilter>& Filters)
{
    return FFilter::Nor(Filters);
}

FFilter UFilterBlueprintLibrary::EqualInt(FName Key, int32 Value)
{
    return FFilter::Equal(Key, Value);
}

FFilter UFilterBlueprintLibrary::EqualString(FName Key, FString Value)
{
    return FFilter::Equal(Key, Value);
}

FFilter UFilterBlueprintLibrary::EqualBool(FName Key, bool bValue)
{
    return FFilter::Equal(Key, bValue);
}

FFilter UFilterBlueprintLibrary::NotEqualInt(FName Key, int32 Value)
{
    return FFilter::NotEqual(Key, Value);
}

FFilter UFilterBlueprintLibrary::NotEqualString(FName Key, FString Value)
{
    return FFilter::NotEqual(Key, Value);
}

FFilter UFilterBlueprintLibrary::NotEqualBool(FName Key, bool bValue)
{
    return FFilter::NotEqual(Key, bValue);
}

FFilter UFilterBlueprintLibrary::GreaterInt(FName Key, int32 Value)
{
    return FFilter::Greater(Key, Value);
}

FFilter UFilterBlueprintLibrary::GreaterOrEqualInt(FName Key, int32 Value)
{
    return FFilter::GreaterOrEqual(Key, Value);
}

FFilter UFilterBlueprintLibrary::LessInt(FName Key, int32 Value)
{
    return FFilter::Less(Key, Value);
}

FFilter UFilterBlueprintLibrary::LessOrEqualInt(FName Key, int32 Value)
{
    return FFilter::LessOrEqual(Key, Value);
}

FFilter UFilterBlueprintLibrary::InInt(FName Key, const TArray<int32>& Values)
{
    return FFilter::In(Key, Values);
}

FFilter UFilterBlueprintLibrary::InString(FName Key, const TArray<FString>& Values)
{
    return FFilter::In(Key, Values);
}

FFilter UFilterBlueprintLibrary::NotInInt(FName Key, const TArray<int32>& Values)
{
    return FFilter::NotIn(Key, Values);
}

FFilter UFilterBlueprintLibrary::NotInString(FName Key, const TArray<FString>& Values)
{
    return FFilter::NotIn(Key, Values);
}

FString UFilterBlueprintLibrary::ToJsonString(const FFilter& Filter)
{
    return Filter.ToJson();
}
