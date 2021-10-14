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
    return Detail::Serialize(T::StaticStruct(), &Struct, NamingConvention);
}

template <class T>
T Deserialize(const FString& Json)
{
    return Detail::Deserialize<T>(Json);
}
}    // namespace Json
