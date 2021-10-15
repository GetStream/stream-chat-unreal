#pragma once

#include "Detail/JsonObjectDeserialization.h"
#include "Detail/JsonObjectSerialization.h"
#include "NamingConvention.h"

class FJsonObject;

namespace Detail
{
STREAMJSON_API FString SerializeJsonObject(const TSharedRef<FJsonObject>&);

template <class T>
FString SerializeStruct(const T& Struct, ENamingConvention NamingConvention)
{
    const TSharedRef<FJsonObject> JsonObject =
        JsonObjectSerialization::UStructToJsonObject(T::StaticStruct(), &Struct, NamingConvention);

    ExtraFields::InvokeSerializeExtra<T>(Struct, *JsonObject);

    return SerializeJsonObject(JsonObject);
}

template <class T>
T DeserializeStruct(const FString& Json)
{
    T OutData;
    JsonObjectDeserialization::JsonObjectStringToUStruct<T>(Json, &OutData);
    return OutData;
}
}    // namespace Detail
