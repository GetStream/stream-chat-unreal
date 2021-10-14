#pragma once

#include "CoreMinimal.h"

#include "Filter.generated.h"

class FJsonValue;
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
 * @remark This is a UObject for easier interaction with Blueprints
 */
UCLASS(BlueprintType)
class UFilter final : public UObject
{
    GENERATED_BODY()

public:
    explicit operator TSharedPtr<FJsonObject>() const;
    explicit operator FJsonObjectWrapper() const;
    FString ToJson() const;

    static UFilter* And(const TArray<UFilter*>& Filters);
    static UFilter* Or(const TArray<UFilter*>& Filters);
    static UFilter* Nor(const TArray<UFilter*>& Filters);

    static UFilter* Equal(const FName& Key, int32 Value);
    static UFilter* Equal(const FName& Key, float Value);
    static UFilter* Equal(const FName& Key, const FString& Value);
    static UFilter* Equal(const FName& Key, bool bValue);
    static UFilter* NotEqual(const FName& Key, int32 Value);
    static UFilter* NotEqual(const FName& Key, float Value);
    static UFilter* NotEqual(const FName& Key, const FString& Value);
    static UFilter* NotEqual(const FName& Key, bool bValue);
    static UFilter* Greater(const FName& Key, int32 Value);
    static UFilter* Greater(const FName& Key, float Value);
    static UFilter* Greater(const FName& Key, const FString& Value);
    static UFilter* Greater(const FName& Key, bool bValue);
    static UFilter* GreaterOrEqual(const FName& Key, int32 Value);
    static UFilter* GreaterOrEqual(const FName& Key, float Value);
    static UFilter* GreaterOrEqual(const FName& Key, const FString& Value);
    static UFilter* GreaterOrEqual(const FName& Key, bool bValue);
    static UFilter* Less(const FName& Key, int32 Value);
    static UFilter* Less(const FName& Key, float Value);
    static UFilter* Less(const FName& Key, const FString& Value);
    static UFilter* Less(const FName& Key, bool bValue);
    static UFilter* LessOrEqual(const FName& Key, int32 Value);
    static UFilter* LessOrEqual(const FName& Key, float Value);
    static UFilter* LessOrEqual(const FName& Key, const FString& Value);
    static UFilter* LessOrEqual(const FName& Key, bool bValue);

    static UFilter* In(const FName& Key, const TArray<int32>& Values);
    static UFilter* In(const FName& Key, const TArray<float>& Values);
    static UFilter* In(const FName& Key, const TArray<FString>& Values);
    static UFilter* NotIn(const FName& Key, const TArray<int32>& Values);
    static UFilter* NotIn(const FName& Key, const TArray<float>& Values);
    static UFilter* NotIn(const FName& Key, const TArray<FString>& Values);

private:
    static UFilter* MakeLogical(EFilterOperator Operator, const TArray<UFilter*>& Filters);
    template <class T>
    static UFilter* MakeComparison(EFilterOperator Operator, const FName& Key, T Value);
    static UFilter* MakeComparison(EFilterOperator Operator, const FName& Key, const FString& Value);
    static UFilter* MakeComparison(EFilterOperator Operator, const FName& Key, bool bValue);
    template <class T>
    static UFilter* MakeArrayComparison(EFilterOperator Operator, const FName& Key, const TArray<T>& Values);
    static UFilter* MakeArrayComparison(EFilterOperator Operator, const FName& Key, const TArray<FString>& Values);

    EFilterOperator Operator;
    FName Key;
    TSharedPtr<FJsonValue> Value;
    UPROPERTY(Transient)
    TArray<UFilter*> ChildFilters;
};

template <class T>
UFilter* UFilter::MakeComparison(const EFilterOperator Operator, const FName& Key, T Value)
{
    UFilter* New = NewObject<UFilter>();
    New->Operator = Operator;
    New->Key = Key;
    New->Value = MakeShared<FJsonValueNumberString>(LexToString(Value));
    return New;
}

template <class T>
UFilter* UFilter::MakeArrayComparison(const EFilterOperator Operator, const FName& Key, const TArray<T>& Values)
{
    UFilter* New = NewObject<UFilter>();
    New->Operator = Operator;
    New->Key = Key;

    TArray<TSharedPtr<FJsonValue>> Array;
    Array.Reserve(Values.Num());
    for (const T& Value : Values)
    {
        Array.Emplace(MakeShared<FJsonValueNumberString>(LexToString(Value)));
    }
    New->Value = MakeShared<FJsonValueArray>(Array);
    return New;
}
