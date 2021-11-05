﻿#pragma once

#include "CoreMinimal.h"
#include "Detail/JsonObjectDeserialization.h"
#include "Detail/JsonObjectSerialization.h"
#include "NamingConvention.h"
#include "UObject/ReflectedTypeAccessors.h"

/**
 * NOTES:
 * You can use the Transient UPROPERTY-specifier to skip Serialization while still performing Deserialization.
 */
namespace Json
{
/**
 * Converts from a UStruct to a Json Object
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param NamingConvention JSON keys and values will be formatted according to this naming convention
 * @return A newly created JSON Object
 */
STREAMJSON_API TSharedRef<FJsonObject> UStructToJsonObject(
    const UStruct* StructDefinition,
    const void* Struct,
    ENamingConvention NamingConvention = ENamingConvention::SnakeCase);

/**
 * Converts from a UStruct to a Json Object
 *
 * @param Struct The UStruct instance to copy out of
 * @param NamingConvention JSON keys and values will be formatted according to this naming convention
 * @return A newly created JSON Object
 */
template <class T>
TSharedRef<FJsonObject> UStructToJsonObject(
    const T& Struct,
    ENamingConvention NamingConvention = ENamingConvention::SnakeCase)
{
    const TSharedRef<FJsonObject> JsonObject = Json::UStructToJsonObject(T::StaticStruct(), &Struct, NamingConvention);
    ExtraFields::InvokeSerializeExtra<T>(Struct, *JsonObject);
    return JsonObject;
}

FString STREAMJSON_API JsonObjectToString(const TSharedRef<FJsonObject>& JsonObject);

template <class T>
typename TEnableIf<TIsClass<T>::Value, FString>::Type Serialize(
    const T& Struct,
    const ENamingConvention NamingConvention = ENamingConvention::SnakeCase)
{
    const TSharedRef<FJsonObject> JsonObject = Json::UStructToJsonObject<T>(Struct, NamingConvention);

    return JsonObjectToString(JsonObject);
}

template <class T>
typename TEnableIf<TIsEnumClass<T>::Value, FString>::Type Serialize(
    const T& Enum,
    const ENamingConvention NamingConvention = ENamingConvention::SnakeCase)
{
    return JsonObjectSerialization::UEnumToString(StaticEnum<T>(), static_cast<int64>(Enum), NamingConvention);
}

template <class T>
T Deserialize(const FString& Json)
{
    T OutData;
    JsonObjectDeserialization::JsonObjectStringToUStruct<T>(Json, &OutData);
    return OutData;
}

template <class T>
void SerializeField(const TOptional<T>& Field, const FString& FieldName, FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        const TSharedRef<FJsonObject> InnerJsonObject = Json::UStructToJsonObject<T>(Field.GetValue());
        JsonObjectSerialization::SetObjectField(JsonObject, FieldName, InnerJsonObject);
    }
}

template <>
void STREAMJSON_API SerializeField(const TOptional<uint32>& Field, const FString& FieldName, FJsonObject&);
template <>
void STREAMJSON_API SerializeField(const TOptional<FString>& Field, const FString& FieldName, FJsonObject&);
template <>
void STREAMJSON_API SerializeField(const TOptional<FDateTime>& Field, const FString& FieldName, FJsonObject&);

template <class T>
void DeserializeField(const FJsonObject& JsonObject, const FString& FieldName, TOptional<T>& Field)
{
    if (const TSharedPtr<FJsonObject>* InnerJsonObject;
        JsonObjectSerialization::TryGetObjectField(JsonObject, FieldName, InnerJsonObject))
    {
        if (T OutStruct; JsonObjectDeserialization::JsonObjectToUStruct(JsonObject, OutStruct))
        {
            Field.Emplace(OutStruct);
        }
    }
}
template <>
void STREAMJSON_API
DeserializeField<uint32>(const FJsonObject& JsonObject, const FString& FieldName, TOptional<uint32>& Field);
template <>
void STREAMJSON_API
DeserializeField<FDateTime>(const FJsonObject& JsonObject, const FString& FieldName, TOptional<FDateTime>& Field);
}    // namespace Json
