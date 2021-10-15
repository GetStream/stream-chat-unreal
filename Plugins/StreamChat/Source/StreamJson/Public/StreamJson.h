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
    return Detail::SerializeStruct(Struct, NamingConvention);
}

template <class T>
T Deserialize(const FString& Json)
{
    return Detail::DeserializeStruct<T>(Json);
}

void STREAMJSON_API SerializeField(const TOptional<uint32>& Field, const FString& FieldName, FJsonObject&);

void STREAMJSON_API DeserializeField(const FJsonObject&, const FString& FieldName, TOptional<uint32>& Field);
}    // namespace Json
