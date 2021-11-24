// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Filter.generated.h"

class FJsonValue;
class FJsonObject;
struct FJsonObjectWrapper;

/**
 * @brief The type of operator of a filter
 * @ingroup StreamChat
 */
enum class EFilterOperator : uint8
{
    /// Matches values that are equal to a specified value.
    Equal,

    /// Matches all values that are not equal to a specified value.
    NotEqual,

    /// Matches values that are greater than a specified value.
    Greater,

    /// Matches values that are greater than a specified value.
    GreaterOrEqual,

    /// Matches values that are less than a specified value.
    Less,

    /// Matches values that are less than or equal to a specified value.
    LessOrEqual,

    /// Matches any of the values specified in an array.
    In,

    /// Matches none of the values specified in an array.
    NotIn,

    /// Matches values by performing text search with the specified value.
    Query,

    /// Matches values with the specified prefix.
    AutoComplete,

    /// Matches values that exist/don't exist based on the specified boolean value.
    Exists,

    /// Matches all the values specified in an array.
    And,

    /// Matches at least one of the values specified in an array.
    Or,

    /// Matches none of the values specified in an array.
    Nor,

    /// Matches any list that contains the specified value
    Contains,
};

/**
 * @brief A filter used for querying channels
 * @see https://getstream.io/chat/docs/other-rest/query_channels
 * @todo Query, AutoComplete, Exists, Contains
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FFilter
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FFilter() = default;
    /// Convert to a JSON object wrapper
    FJsonObjectWrapper ToJsonObjectWrapper() const;
    /// Convert to a JSON string
    FString ToJson() const;

    /// Construct an $and filter
    static FFilter And(const TArray<FFilter>& Filters);
    /// Construct an $or filter
    static FFilter Or(const TArray<FFilter>& Filters);
    /// Construct a $nor filter
    static FFilter Nor(const TArray<FFilter>& Filters);

    /// Construct an $eq filter
    static FFilter Equal(const FName& Key, int32 Value);
    /// Construct an $eq filter
    static FFilter Equal(const FName& Key, float Value);
    /// Construct an $eq filter
    static FFilter Equal(const FName& Key, const FString& Value);
    /// Construct an $eq filter
    static FFilter Equal(const FName& Key, bool bValue);
    /// Construct a $neq filter
    static FFilter NotEqual(const FName& Key, int32 Value);
    /// Construct a $neq filter
    static FFilter NotEqual(const FName& Key, float Value);
    /// Construct a $neq filter
    static FFilter NotEqual(const FName& Key, const FString& Value);
    /// Construct a $neq filter
    static FFilter NotEqual(const FName& Key, bool bValue);
    /// Construct a $gt filter
    static FFilter Greater(const FName& Key, int32 Value);
    /// Construct a $gt filter
    static FFilter Greater(const FName& Key, float Value);
    /// Construct a $gt filter
    static FFilter Greater(const FName& Key, const FString& Value);
    /// Construct a $gt filter
    static FFilter Greater(const FName& Key, bool bValue);
    /// Construct a $gte filter
    static FFilter GreaterOrEqual(const FName& Key, int32 Value);
    /// Construct a $gte filter
    static FFilter GreaterOrEqual(const FName& Key, float Value);
    /// Construct a $gte filter
    static FFilter GreaterOrEqual(const FName& Key, const FString& Value);
    /// Construct a $gte filter
    static FFilter GreaterOrEqual(const FName& Key, bool bValue);
    /// Construct a $lt filter
    static FFilter Less(const FName& Key, int32 Value);
    /// Construct a $lt filter
    static FFilter Less(const FName& Key, float Value);
    /// Construct a $lt filter
    static FFilter Less(const FName& Key, const FString& Value);
    /// Construct a $lt filter
    static FFilter Less(const FName& Key, bool bValue);
    /// Construct a $lte filter
    static FFilter LessOrEqual(const FName& Key, int32 Value);
    /// Construct a $lte filter
    static FFilter LessOrEqual(const FName& Key, float Value);
    /// Construct a $lte filter
    static FFilter LessOrEqual(const FName& Key, const FString& Value);
    /// Construct a $lte filter
    static FFilter LessOrEqual(const FName& Key, bool bValue);

    /// Construct an $in filter
    static FFilter In(const FName& Key, const TArray<int32>& Values);
    /// Construct an $in filter
    static FFilter In(const FName& Key, const TArray<float>& Values);
    /// Construct an $in filter
    static FFilter In(const FName& Key, const TArray<FString>& Values);
    /// Construct an $in filter
    static FFilter NotIn(const FName& Key, const TArray<int32>& Values);
    /// Construct an $nin filter
    static FFilter NotIn(const FName& Key, const TArray<float>& Values);
    /// Construct an $nin filter
    static FFilter NotIn(const FName& Key, const TArray<FString>& Values);

    /// Check if this filter has been correctly initialized
    bool IsValid() const;

private:
    /// Make logical filter
    FFilter(EFilterOperator Operator, const TArray<FFilter>& Filters);
    /// Make comparison filter
    FFilter(EFilterOperator Operator, const FName& Key, const TSharedPtr<FJsonValue>& Value);

    TSharedPtr<FJsonObject> ToJsonObject() const;
    template <class T>
    static FFilter MakeComparison(EFilterOperator Operator, const FName& Key, T Value);
    static FFilter MakeComparison(EFilterOperator Operator, const FName& Key, const FString& Value);
    static FFilter MakeComparison(EFilterOperator Operator, const FName& Key, bool bValue);
    template <class T>
    static FFilter MakeArrayComparison(EFilterOperator Operator, const FName& Key, const TArray<T>& Values);
    static FFilter MakeArrayComparison(EFilterOperator Operator, const FName& Key, const TArray<FString>& Values);

    EFilterOperator Operator;
    FName Key;
    TSharedPtr<FJsonValue> Value;
    TArray<FFilter> ChildFilters;
};
