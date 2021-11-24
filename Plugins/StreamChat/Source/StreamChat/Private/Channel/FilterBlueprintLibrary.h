// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Filter.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FilterBlueprintLibrary.generated.h"

UCLASS()
class STREAMCHAT_API UFilterBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter")
    static FString ToJsonString(const FFilter& Filter);

    /// Matches all the values specified in an array.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$and (Filter)", CompactNodeTitle = "$and", Keywords = "&"))
    static FFilter And(const TArray<FFilter>& Filters);
    /// Matches at least one of the values specified in an array.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$or (Filter)", CompactNodeTitle = "$or", Keywords = "|"))
    static FFilter Or(const TArray<FFilter>& Filters);
    /// Matches none of the values specified in an array.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$nor (Filter)", CompactNodeTitle = "$nor", Keywords = "not |"))
    static FFilter Nor(const TArray<FFilter>& Filters);

    /// Matches values that are equal to a specified value.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$eq (Filter, integer)", Keywords = "= $eq"))
    static FFilter EqualInt(FName Key, int32 Value);
    /// Matches values that are equal to a specified value.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$eq (Filter, string)", Keywords = "= $eq"))
    static FFilter EqualString(FName Key, FString Value);
    /// Matches values that are equal to a specified value.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$eq (Filter, boolean)", Keywords = "= $eq"))
    static FFilter EqualBool(FName Key, bool bValue);

    /// Matches all values that are not equal to a specified value. This is not supported for custom fields.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$ne (Filter, integer)", Keywords = "!= $neq"))
    static FFilter NotEqualInt(FName Key, int32 Value);
    /// Matches all values that are not equal to a specified value. This is not supported for custom fields.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$ne (Filter, string)", Keywords = "!= $neq"))
    static FFilter NotEqualString(FName Key, FString Value);
    /// Matches all values that are not equal to a specified value. This is not supported for custom fields.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$ne (Filter, boolean)", Keywords = "!= $neq"))
    static FFilter NotEqualBool(FName Key, bool bValue);

    /// Matches values that are greater than a specified value.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$gt (Filter)", Keywords = "> $gt"))
    static FFilter GreaterInt(FName Key, int32 Value);

    /// Matches values that are greater than or equal to a specified value.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$gte (Filter)", Keywords = ">= $gte"))
    static FFilter GreaterOrEqualInt(FName Key, int32 Value);

    /// Matches values that are less than a specified value.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$lt (Filter)", Keywords = "> $lt"))
    static FFilter LessInt(FName Key, int32 Value);

    /// Matches values that are less than or equal to a specified value.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$lte (Filter)", Keywords = ">= $lte"))
    static FFilter LessOrEqualInt(FName Key, int32 Value);

    /// Matches any of the values specified in an array.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$in (Filter, integer)"))
    static FFilter InInt(FName Key, const TArray<int32>& Values);
    /// Matches any of the values specified in an array.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$in (Filter, string)"))
    static FFilter InString(FName Key, const TArray<FString>& Values);
    /// Matches none of the values specified in an array.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$nin (Filter, integer)", Keywords = "not"))
    static FFilter NotInInt(FName Key, const TArray<int32>& Values);
    /// Matches none of the values specified in an array.
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Filter", meta = (DisplayName = "$nin (Filter, string)", Keywords = "not"))
    static FFilter NotInString(FName Key, const TArray<FString>& Values);
};
