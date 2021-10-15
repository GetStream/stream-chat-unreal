#pragma once

#include "CoreMinimal.h"

#include "Filter.generated.h"

class FJsonValue;
class FJsonObject;
struct FJsonObjectWrapper;

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
 * TODO Query, AutoComplete, Exists, Contains
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FFilter
{
    GENERATED_BODY()

    // USTRUCTs need a default constructor
    FFilter() = default;
    // Make logical filter
    FFilter(EFilterOperator Operator, const TArray<FFilter>& Filters);
    // Make comparison filter
    FFilter(EFilterOperator Operator, const FName& Key, const TSharedPtr<FJsonValue>& Value);

    explicit operator TSharedPtr<FJsonObject>() const;
    explicit operator FJsonObjectWrapper() const;
    FString ToJson() const;

    static FFilter And(const TArray<FFilter>& Filters);
    static FFilter Or(const TArray<FFilter>& Filters);
    static FFilter Nor(const TArray<FFilter>& Filters);

    static FFilter Equal(const FName& Key, int32 Value);
    static FFilter Equal(const FName& Key, float Value);
    static FFilter Equal(const FName& Key, const FString& Value);
    static FFilter Equal(const FName& Key, bool bValue);
    static FFilter NotEqual(const FName& Key, int32 Value);
    static FFilter NotEqual(const FName& Key, float Value);
    static FFilter NotEqual(const FName& Key, const FString& Value);
    static FFilter NotEqual(const FName& Key, bool bValue);
    static FFilter Greater(const FName& Key, int32 Value);
    static FFilter Greater(const FName& Key, float Value);
    static FFilter Greater(const FName& Key, const FString& Value);
    static FFilter Greater(const FName& Key, bool bValue);
    static FFilter GreaterOrEqual(const FName& Key, int32 Value);
    static FFilter GreaterOrEqual(const FName& Key, float Value);
    static FFilter GreaterOrEqual(const FName& Key, const FString& Value);
    static FFilter GreaterOrEqual(const FName& Key, bool bValue);
    static FFilter Less(const FName& Key, int32 Value);
    static FFilter Less(const FName& Key, float Value);
    static FFilter Less(const FName& Key, const FString& Value);
    static FFilter Less(const FName& Key, bool bValue);
    static FFilter LessOrEqual(const FName& Key, int32 Value);
    static FFilter LessOrEqual(const FName& Key, float Value);
    static FFilter LessOrEqual(const FName& Key, const FString& Value);
    static FFilter LessOrEqual(const FName& Key, bool bValue);

    static FFilter In(const FName& Key, const TArray<int32>& Values);
    static FFilter In(const FName& Key, const TArray<float>& Values);
    static FFilter In(const FName& Key, const TArray<FString>& Values);
    static FFilter NotIn(const FName& Key, const TArray<int32>& Values);
    static FFilter NotIn(const FName& Key, const TArray<float>& Values);
    static FFilter NotIn(const FName& Key, const TArray<FString>& Values);

private:
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
