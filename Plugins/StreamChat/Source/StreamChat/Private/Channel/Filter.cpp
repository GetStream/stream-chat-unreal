// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Filter.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "JsonObjectWrapper.h"
#include "NamingConventionConversion.h"
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

FFilter::FFilter(const EFilterOperator Operator, const TArray<FFilter>& Filters) : Operator{Operator}, ChildFilters{Filters}
{
}

FFilter::FFilter(const EFilterOperator Operator, const FName& Field, const TSharedPtr<FJsonValue>& Value) : Operator{Operator}, Field{Field}, Value{Value}
{
}

TSharedRef<FJsonObject> FFilter::ToJsonObject() const
{
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();

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
                Array.Emplace(MakeShared<FJsonValueObject>(Child.ToJsonObject()));
            }
        }
        JsonObject->SetArrayField(AsString[Operator], Array);
    }
    else if (!Field.IsNone())
    {
        if (Operator == EFilterOperator::None)
        {
            // None filters are encoded in the following form:
            // { <field>: {} }
            JsonObject->SetField(Field.ToString(), MakeShared<FJsonValueObject>(MakeShared<FJsonObject>()));
        }
        else
        {
            // Normal filters are encoded in the following form:
            // { key: { $<operator>: <value> } }
            const TSharedPtr<FJsonObject> InnerObject = MakeShared<FJsonObject>();
            InnerObject->SetField(AsString[Operator], Value);
            const FString Key = NamingConventionConversion::ConvertPropertyNameToSnakeCase(Field.ToString());
            JsonObject->SetObjectField(Key, InnerObject);
        }
    }

    return JsonObject;
}

FJsonObjectWrapper FFilter::ToJsonObjectWrapper() const
{
    FJsonObjectWrapper Wrapper;
    Wrapper.JsonObject = ToJsonObject();
    return Wrapper;
}

FString FFilter::ToJson() const
{
    return JsonObject::JsonObjectToString(ToJsonObject());
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

FFilter FFilter::Equal(const FName& Field, const int32 Value)
{
    return MakeComparison(EFilterOperator::Equal, Field, Value);
}

FFilter FFilter::Equal(const FName& Field, const float Value)
{
    return MakeComparison(EFilterOperator::Equal, Field, Value);
}

FFilter FFilter::Equal(const FName& Field, const FString& Value)
{
    return MakeComparison(EFilterOperator::Equal, Field, Value);
}

FFilter FFilter::Equal(const FName& Field, const bool bValue)
{
    return MakeComparison(EFilterOperator::Equal, Field, bValue);
}

FFilter FFilter::NotEqual(const FName& Field, const int32 Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Field, Value);
}

FFilter FFilter::NotEqual(const FName& Field, const float Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Field, Value);
}

FFilter FFilter::NotEqual(const FName& Field, const FString& Value)
{
    return MakeComparison(EFilterOperator::NotEqual, Field, Value);
}

FFilter FFilter::NotEqual(const FName& Field, const bool bValue)
{
    return MakeComparison(EFilterOperator::NotEqual, Field, bValue);
}

FFilter FFilter::Greater(const FName& Field, int32 const Value)
{
    return MakeComparison(EFilterOperator::Greater, Field, Value);
}

FFilter FFilter::Greater(const FName& Field, float const Value)
{
    return MakeComparison(EFilterOperator::Greater, Field, Value);
}

FFilter FFilter::Greater(const FName& Field, const FString& Value)
{
    return MakeComparison(EFilterOperator::Greater, Field, Value);
}

FFilter FFilter::Greater(const FName& Field, bool const bValue)
{
    return MakeComparison(EFilterOperator::Greater, Field, bValue);
}

FFilter FFilter::GreaterOrEqual(const FName& Field, int32 const Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Field, Value);
}

FFilter FFilter::GreaterOrEqual(const FName& Field, float const Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Field, Value);
}

FFilter FFilter::GreaterOrEqual(const FName& Field, const FString& Value)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Field, Value);
}

FFilter FFilter::GreaterOrEqual(const FName& Field, bool const bValue)
{
    return MakeComparison(EFilterOperator::GreaterOrEqual, Field, bValue);
}

FFilter FFilter::Less(const FName& Field, int32 const Value)
{
    return MakeComparison(EFilterOperator::Less, Field, Value);
}

FFilter FFilter::Less(const FName& Field, float const Value)
{
    return MakeComparison(EFilterOperator::Less, Field, Value);
}

FFilter FFilter::Less(const FName& Field, const FString& Value)
{
    return MakeComparison(EFilterOperator::Less, Field, Value);
}

FFilter FFilter::Less(const FName& Field, bool const bValue)
{
    return MakeComparison(EFilterOperator::Less, Field, bValue);
}

FFilter FFilter::LessOrEqual(const FName& Field, int32 const Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Field, Value);
}

FFilter FFilter::LessOrEqual(const FName& Field, float const Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Field, Value);
}

FFilter FFilter::LessOrEqual(const FName& Field, const FString& Value)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Field, Value);
}

FFilter FFilter::LessOrEqual(const FName& Field, bool const bValue)
{
    return MakeComparison(EFilterOperator::LessOrEqual, Field, bValue);
}

FFilter FFilter::In(const FName& Field, const TArray<int32>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Field, Values);
}

FFilter FFilter::In(const FName& Field, const TArray<float>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Field, Values);
}

FFilter FFilter::In(const FName& Field, const TArray<FString>& Values)
{
    return MakeArrayComparison(EFilterOperator::In, Field, Values);
}

FFilter FFilter::NotIn(const FName& Field, const TArray<int32>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Field, Values);
}

FFilter FFilter::NotIn(const FName& Field, const TArray<float>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Field, Values);
}

FFilter FFilter::NotIn(const FName& Field, const TArray<FString>& Values)
{
    return MakeArrayComparison(EFilterOperator::NotIn, Field, Values);
}

FFilter FFilter::Empty(const FName& Field)
{
    FFilter Filter;
    Filter.Operator = EFilterOperator::None;
    Filter.Field = Field;
    return Filter;
}

FFilter FFilter::MakeComparison(const EFilterOperator Operator, const FName& Field, const FString& Value)
{
    return {Operator, Field, MakeShared<FJsonValueString>(Value)};
}

FFilter FFilter::MakeComparison(const EFilterOperator Operator, const FName& Field, bool bValue)
{
    return {Operator, Field, MakeShared<FJsonValueBoolean>(bValue)};
}

FFilter FFilter::MakeArrayComparison(const EFilterOperator Operator, const FName& Field, const TArray<FString>& Values)
{
    TArray<TSharedPtr<FJsonValue>> Array;
    Array.Reserve(Values.Num());
    for (const FString& Value : Values)
    {
        Array.Emplace(MakeShared<FJsonValueString>(Value));
    }
    return {Operator, Field, MakeShared<FJsonValueArray>(Array)};
}

bool FFilter::IsValid() const
{
    return ChildFilters.Num() > 0 || (!Field.IsNone() && Value.IsValid());
}

template <class T>
FFilter FFilter::MakeComparison(const EFilterOperator Operator, const FName& Field, T Value)
{
    return {Operator, Field, MakeShared<FJsonValueNumberString>(LexToString(Value))};
}

template <class T>
FFilter FFilter::MakeArrayComparison(const EFilterOperator Operator, const FName& Field, const TArray<T>& Values)
{
    TArray<TSharedPtr<FJsonValue>> Array;
    Array.Reserve(Values.Num());
    for (const T& Value : Values)
    {
        Array.Emplace(MakeShared<FJsonValueNumberString>(LexToString(Value)));
    }
    return {Operator, Field, MakeShared<FJsonValueArray>(Array)};
}
