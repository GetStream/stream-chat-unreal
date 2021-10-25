#pragma once

#include "CoreMinimal.h"
#include "Detail/JsonObjectDeserialization.h"
#include "Detail/JsonObjectSerialization.h"
#include "NamingConvention.h"

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

FString STREAMJSON_API JsonObjectToString(const TSharedRef<FJsonObject>& JsonObject);

template <class T>
FString Serialize(const T& Struct, ENamingConvention NamingConvention = ENamingConvention::SnakeCase)
{
    const TSharedRef<FJsonObject> JsonObject = Json::UStructToJsonObject(T::StaticStruct(), &Struct, NamingConvention);

    ExtraFields::InvokeSerializeExtra<T>(Struct, *JsonObject);

    return JsonObjectToString(JsonObject);
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
        const TSharedRef<FJsonObject> InnerJsonObject = Json::UStructToJsonObject(T::StaticStruct(), &Field.GetValue());
        JsonObjectSerialization::SetObjectField(JsonObject, FieldName, InnerJsonObject);
    }
}

template <>
void STREAMJSON_API SerializeField(const TOptional<uint32>& Field, const FString& FieldName, FJsonObject&);
template <>
void STREAMJSON_API SerializeField(const TOptional<FString>& Field, const FString& FieldName, FJsonObject&);
template <>
void STREAMJSON_API SerializeField(const TOptional<FDateTime>& Field, const FString& FieldName, FJsonObject&);

void STREAMJSON_API DeserializeField(const FJsonObject&, const FString& FieldName, TOptional<uint32>& Field);
}    // namespace Json
