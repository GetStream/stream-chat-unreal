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

UFilter::operator TSharedPtr<FJsonObject>() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

    // Logical (group) operators
    if (ChildFilters.Num() > 0)
    {
        // Filters with group operators are encoded in the following form:
        // { $<operator>: [ <filter 1>, <filter 2> ] }
        TArray<TSharedPtr<FJsonValue>> Array;
        Array.Reserve(ChildFilters.Num());
        for (const UFilter* Child : ChildFilters)
        {
            Array.Emplace(MakeShared<FJsonValueObject>(static_cast<TSharedPtr<FJsonObject>>(*Child)));
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

UFilter::operator FJsonObjectWrapper() const
{
    FJsonObjectWrapper Wrapper;
    Wrapper.JsonObject = static_cast<TSharedPtr<FJsonObject>>(*this);
    return Wrapper;
}

FString UFilter::ToJson() const
{
    return Json::Serialize(static_cast<TSharedPtr<FJsonObject>>(*this).ToSharedRef());
}

UFilter* UFilter::And(const TArray<UFilter*>& Filters)
{
    return MakeLogical(EFilterOperator::And, Filters);
}

UFilter* UFilter::Or(const TArray<UFilter*>& Filters)
{
    return MakeLogical(EFilterOperator::Or, Filters);
}

UFilter* UFilter::Nor(const TArray<UFilter*>& Filters)
{
    return MakeLogical(EFilterOperator::Nor, Filters);
}

UFilter* UFilter::Equal(const FName& Key, const int32 Value)
{
    return MakeComparison(EFilterOperator::Equal, Key, Value);
}

UFilter* UFilter::Equal(const FName& Key, const float Value)
{
    return MakeComparison(EFilterOperator::Equal, Key, Value);
}

UFilter* UFilter::Equal(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::Equal, Key, Value);
}

UFilter* UFilter::Equal(const FName& Key, const bool bValue)
{
    return MakeComparison(EFilterOperator::Equal, Key, bValue);
}

UFilter* UFilter::NotEqual(const FName& Key, const int32 Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Key, Value);
}

UFilter* UFilter::NotEqual(const FName& Key, const float Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Key, Value);
}

UFilter* UFilter::NotEqual(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Key, Value);
}

UFilter* UFilter::NotEqual(const FName& Key, const bool bValue)
{
    return MakeComparison(EFilterOperator::NotEqual, Key, bValue);
}

UFilter* UFilter::Greater(const FName& Key, int32 const Value)
{
    return MakeComparison(EFilterOperator::Greater, Key, Value);
}

UFilter* UFilter::Greater(const FName& Key, float const Value)
{
    return MakeComparison(EFilterOperator::Greater, Key, Value);
}

UFilter* UFilter::Greater(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::Greater, Key, Value);
}

UFilter* UFilter::Greater(const FName& Key, bool const bValue)
{
    return MakeComparison(EFilterOperator::Greater, Key, bValue);
}

UFilter* UFilter::GreaterOrEqual(const FName& Key, int32 const Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Key, Value);
}

UFilter* UFilter::GreaterOrEqual(const FName& Key, float const Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Key, Value);
}

UFilter* UFilter::GreaterOrEqual(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Key, Value);
}

UFilter* UFilter::GreaterOrEqual(const FName& Key, bool const bValue)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Key, bValue);
}

UFilter* UFilter::Less(const FName& Key, int32 const Value)
{
    return MakeComparison(EFilterOperator::Less, Key, Value);
}

UFilter* UFilter::Less(const FName& Key, float const Value)
{
    return MakeComparison(EFilterOperator::Less, Key, Value);
}

UFilter* UFilter::Less(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::Less, Key, Value);
}

UFilter* UFilter::Less(const FName& Key, bool const bValue)
{
    return MakeComparison(EFilterOperator::Less, Key, bValue);
}

UFilter* UFilter::LessOrEqual(const FName& Key, int32 const Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Key, Value);
}

UFilter* UFilter::LessOrEqual(const FName& Key, float const Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Key, Value);
}

UFilter* UFilter::LessOrEqual(const FName& Key, const FString& Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Key, Value);
}

UFilter* UFilter::LessOrEqual(const FName& Key, bool const bValue)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Key, bValue);
}

UFilter* UFilter::In(const FName& Key, const TArray<int32>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Key, Values);
}

UFilter* UFilter::In(const FName& Key, const TArray<float>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Key, Values);
}

UFilter* UFilter::In(const FName& Key, const TArray<FString>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Key, Values);
}

UFilter* UFilter::NotIn(const FName& Key, const TArray<int32>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Key, Values);
}

UFilter* UFilter::NotIn(const FName& Key, const TArray<float>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Key, Values);
}

UFilter* UFilter::NotIn(const FName& Key, const TArray<FString>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Key, Values);
}

UFilter* UFilter::MakeLogical(const EFilterOperator Operator, const TArray<UFilter*>& Filters)
{
    UFilter* New = NewObject<UFilter>();
    New->Operator = Operator;
    New->ChildFilters = Filters;
    return New;
}

UFilter* UFilter::MakeComparison(const EFilterOperator Operator, const FName& Key, const FString& Value)
{
    UFilter* New = NewObject<UFilter>();
    New->Operator = Operator;
    New->Key = Key;
    New->Value = MakeShared<FJsonValueString>(Value);
    return New;
}

UFilter* UFilter::MakeComparison(const EFilterOperator Operator, const FName& Key, bool bValue)
{
    UFilter* New = NewObject<UFilter>();
    New->Operator = Operator;
    New->Key = Key;
    New->Value = MakeShared<FJsonValueBoolean>(bValue);
    return New;
}

UFilter* UFilter::MakeArrayComparison(const EFilterOperator Operator, const FName& Key, const TArray<FString>& Values)
{
    UFilter* New = NewObject<UFilter>();
    New->Operator = Operator;
    New->Key = Key;

    TArray<TSharedPtr<FJsonValue>> Array;
    Array.Reserve(Values.Num());
    for (const FString& Value : Values)
    {
        Array.Emplace(MakeShared<FJsonValueString>(Value));
    }
    New->Value = MakeShared<FJsonValueArray>(Array);
    return New;
}

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
