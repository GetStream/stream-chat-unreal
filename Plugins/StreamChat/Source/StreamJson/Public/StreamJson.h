#pragma once

#include "CoreMinimal.h"
#include "Detail/JsonDetail.h"
#include "NamingConvention.h"

namespace Json
{
FString STREAMJSON_API Serialize(const TSharedRef<FJsonObject>& JsonObject);

template <class T>
FString Serialize(const T& Struct, ENamingConvention NamingConvention = ENamingConvention::SnakeCase)
{
    return Detail::Serialize(Struct, NamingConvention);
}

template <class T>
T Deserialize(const FString& Json)
{
    return Detail::Deserialize<T>(Json);
}

void STREAMJSON_API SerializeField(const TOptional<uint32>& Field, const FString& FieldName, FJsonObject& JsonObject);
void STREAMJSON_API DeserializeField(const FJsonObject& JsonObject, const FString& FieldName, TOptional<uint32>& Field);
}    // namespace Json
