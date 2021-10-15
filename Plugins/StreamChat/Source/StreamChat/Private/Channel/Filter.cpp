#include "Channel/Filter.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "JsonObjectWrapper.h"
#include "StreamJson.h"

namespace
{
const TMap<EFilterOperator, FString> AsString = {
    {EFilterOperator::Equal, TEXT("$eq")},
    {EFilterOperator::NotEqual, TEXT("$ne")},
    {EFilterOperator::Greater, TEXT("$gt")},
    {EFilterOperator::GreaterOrEqual, TEXT("$gte")},
    {EFilterOperator::Less, TEXT("$lt")},
    {EFilterOperator::LessOrEqual, TEXT("$lte")},
    {EFilterOperator::In, TEXT("$in")},
    {EFilterOperator::NotIn, TEXT("$nin")},
    {EFilterOperator::Query, TEXT("$q")},
    {EFilterOperator::AutoComplete, TEXT("$autocomplete")},
    {EFilterOperator::Exists, TEXT("$exists")},
    {EFilterOperator::And, TEXT("$and")},
    {EFilterOperator::Or, TEXT("$or")},
    {EFilterOperator::Nor, TEXT("$nor")},
    {EFilterOperator::Contains, TEXT("$contains")},
};

}    // namespace

FFilter::FFilter(const EFilterOperator Operator, const TArray<FFilter>& Filters)
    : Operator{Operator}, ChildFilters{Filters}
{
}

FFilter::FFilter(const EFilterOperator Operator, const FName& Key, const TSharedPtr<FJsonValue>& Value)
    : Operator{Operator}, Key{Key}, Value{Value}
{
}

FFilter::operator TSharedPtr<FJsonObject>() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

    // Logical (group) operators
    if (ChildFilters.Num() > 0)
    {
        // Filters with group operators are encoded in the following form:
        // { $<operator>: [ <filter 1>, <filter 2> ] }
        TArray<TSharedPtr<FJsonValue>> Array;
        Array.Reserve(ChildFilters.Num());
        for (const FFilter& Child : ChildFilters)
        {
            if (Child.IsValid())
            {
                Array.Emplace(MakeShared<FJsonValueObject>(static_cast<TSharedPtr<FJsonObject>>(Child)));
            }
        }
        JsonObject->SetArrayField(AsString[Operator], Array);
    }
    else if (!Key.IsNone())
    {
        // Normal filters are encoded in the following form:
        // { key: { $<operator>: <value> } }
        const TSharedPtr<FJsonObject> InnerObject = MakeShared<FJsonObject>();
        InnerObject->SetField(AsString[Operator], Value);
        JsonObject->SetObjectField(Key.ToString(), InnerObject);
    }

    return JsonObject;
}

FFilter::operator FJsonObjectWrapper() const
{
    FJsonObjectWrapper Wrapper;
    Wrapper.JsonObject = static_cast<TSharedPtr<FJsonObject>>(*this);
    return Wrapper;
}

FString FFilter::ToJson() const
{
    return Json::Serialize(static_cast<TSharedPtr<FJsonObject>>(*this).ToSharedRef());
}

FFilter FFilter::And(const TArray<FFilter>& Filters)
{
    return {EFilterOperator::And, Filters};
}

FFilter FFilter::Or(const TArray<FFilter>& Filters)
{
    return {EFilterOperator::Or, Filters};
}

FFilter FFilter::Nor(const TArray<FFilter>& Filters)
{
    return {EFilterOperator::Nor, Filters};
}

FFilter FFilter::Equal(const FName& Key, const int32 Value)
{
    return MakeComparison(EFilterOperator::Equal, Key, Value);
}

FFilter FFilter::Equal(const FName& Key, const float Value)
{
    return MakeComparison(EFilterOperator::Equal, Key, Value);
}

FFilter FFilter::Equal(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::Equal, Key, Value);
}

FFilter FFilter::Equal(const FName& Key, const bool bValue)
{
    return MakeComparison(EFilterOperator::Equal, Key, bValue);
}

FFilter FFilter::NotEqual(const FName& Key, const int32 Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Key, Value);
}

FFilter FFilter::NotEqual(const FName& Key, const float Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Key, Value);
}

FFilter FFilter::NotEqual(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Key, Value);
}

FFilter FFilter::NotEqual(const FName& Key, const bool bValue)
{
    return MakeComparison(EFilterOperator::NotEqual, Key, bValue);
}

FFilter FFilter::Greater(const FName& Key, int32 const Value)
{
    return MakeComparison(EFilterOperator::Greater, Key, Value);
}

FFilter FFilter::Greater(const FName& Key, float const Value)
{
    return MakeComparison(EFilterOperator::Greater, Key, Value);
}

FFilter FFilter::Greater(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::Greater, Key, Value);
}

FFilter FFilter::Greater(const FName& Key, bool const bValue)
{
    return MakeComparison(EFilterOperator::Greater, Key, bValue);
}

FFilter FFilter::GreaterOrEqual(const FName& Key, int32 const Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Key, Value);
}

FFilter FFilter::GreaterOrEqual(const FName& Key, float const Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Key, Value);
}

FFilter FFilter::GreaterOrEqual(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Key, Value);
}

FFilter FFilter::GreaterOrEqual(const FName& Key, bool const bValue)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Key, bValue);
}

FFilter FFilter::Less(const FName& Key, int32 const Value)
{
    return MakeComparison(EFilterOperator::Less, Key, Value);
}

FFilter FFilter::Less(const FName& Key, float const Value)
{
    return MakeComparison(EFilterOperator::Less, Key, Value);
}

FFilter FFilter::Less(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::Less, Key, Value);
}

FFilter FFilter::Less(const FName& Key, bool const bValue)
{
    return MakeComparison(EFilterOperator::Less, Key, bValue);
}

FFilter FFilter::LessOrEqual(const FName& Key, int32 const Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Key, Value);
}

FFilter FFilter::LessOrEqual(const FName& Key, float const Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Key, Value);
}

FFilter FFilter::LessOrEqual(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Key, Value);
}

FFilter FFilter::LessOrEqual(const FName& Key, bool const bValue)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Key, bValue);
}

FFilter FFilter::In(const FName& Key, const TArray<int32>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Key, Values);
}

FFilter FFilter::In(const FName& Key, const TArray<float>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Key, Values);
}

FFilter FFilter::In(const FName& Key, const TArray<FString>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Key, Values);
}

FFilter FFilter::NotIn(const FName& Key, const TArray<int32>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Key, Values);
}

FFilter FFilter::NotIn(const FName& Key, const TArray<float>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Key, Values);
}

FFilter FFilter::NotIn(const FName& Key, const TArray<FString>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Key, Values);
}

FFilter FFilter::MakeComparison(const EFilterOperator Operator, const FName& Key, const FString& Value)
{
    return {Operator, Key, MakeShared<FJsonValueString>(Value)};
}

FFilter FFilter::MakeComparison(const EFilterOperator Operator, const FName& Key, bool bValue)
{
    return {Operator, Key, MakeShared<FJsonValueBoolean>(bValue)};
}

FFilter FFilter::MakeArrayComparison(const EFilterOperator Operator, const FName& Key, const TArray<FString>& Values)
{
    TArray<TSharedPtr<FJsonValue>> Array;
    Array.Reserve(Values.Num());
    for (const FString& Value : Values)
    {
        Array.Emplace(MakeShared<FJsonValueString>(Value));
    }
    return {Operator, Key, MakeShared<FJsonValueArray>(Array)};
}

bool FFilter::IsValid() const
{
    return ChildFilters.Num() > 0 || (!Key.IsNone() && Value.IsValid());
}

template <class T>
FFilter FFilter::MakeComparison(const EFilterOperator Operator, const FName& Key, T Value)
{
    return {Operator, Key, MakeShared<FJsonValueNumberString>(LexToString(Value))};
}

template <class T>
FFilter FFilter::MakeArrayComparison(const EFilterOperator Operator, const FName& Key, const TArray<T>& Values)
{
    TArray<TSharedPtr<FJsonValue>> Array;
    Array.Reserve(Values.Num());
    for (const T& Value : Values)
    {
        Array.Emplace(MakeShared<FJsonValueNumberString>(LexToString(Value)));
    }
    return {Operator, Key, MakeShared<FJsonValueArray>(Array)};
}
