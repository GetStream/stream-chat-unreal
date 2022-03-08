// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Detail/JsonObjectDeserialization.h"
#include "Detail/JsonObjectSerialization.h"
#include "StreamJson.h"

/**
 * @brief Custom serialization and deserialization for types which aren't supported by UPROPERTY (e.g. TOptional)
 * @ingroup StreamChatJson
 */
namespace JsonField
{
template <class T>
void Serialize(const TOptional<T>& Field, const FString& FieldName, FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        const TSharedRef<FJsonObject> InnerJsonObject = JsonObject::UStructToJsonObject<T>(Field.GetValue());
        JsonObjectSerialization::SetObjectField(JsonObject, FieldName, InnerJsonObject);
    }
}

template <>
void STREAMJSON_API Serialize(const TOptional<uint32>& Field, const FString& FieldName, FJsonObject&);
template <>
void STREAMJSON_API Serialize(const TOptional<FString>& Field, const FString& FieldName, FJsonObject&);
template <>
void STREAMJSON_API Serialize(const TOptional<FDateTime>& Field, const FString& FieldName, FJsonObject&);

template <class T>
void Deserialize(const FJsonObject& JsonObject, const FString& FieldName, TOptional<T>& Field)
{
    const TSharedPtr<FJsonObject>* InnerJsonObject;
    if (JsonObjectSerialization::TryGetObjectField(JsonObject, FieldName, InnerJsonObject))
    {
        T OutStruct;
        if (JsonObjectDeserialization::JsonObjectToUStruct(JsonObject, OutStruct))
        {
            Field.Emplace(OutStruct);
        }
    }
}
template <>
void STREAMJSON_API Deserialize<uint32>(const FJsonObject& JsonObject, const FString& FieldName, TOptional<uint32>& Field);
template <>
void STREAMJSON_API Deserialize<FDateTime>(const FJsonObject& JsonObject, const FString& FieldName, TOptional<FDateTime>& Field);
}    // namespace JsonField
