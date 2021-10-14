#pragma once

#include "Detail/JsonObjectDeserialization.h"
#include "Detail/JsonObjectSerialization.h"
#include "NamingConvention.h"
#include "StreamJson.h"

class FJsonObject;

namespace Detail
{
template <class T>
FString Serialize(const T& Struct, ENamingConvention NamingConvention)
{
    const TSharedRef<FJsonObject> JsonObject =
        JsonObjectSerialization::UStructToJsonObject(T::StaticStruct(), &Struct, NamingConvention);

    ExtraFields::InvokeSerializeExtra<T>(Struct, *JsonObject);

    return Json::Serialize(JsonObject);
}

template <class T>
T Deserialize(const FString& Json)
{
    T OutData;
    JsonObjectDeserialization::JsonObjectStringToUStruct<T>(Json, &OutData);
    return OutData;
}
}    // namespace Detail
