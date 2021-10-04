#pragma once

#include "Detail/JsonObjectDeserialization.h"
#include "NamingConvention.h"

namespace Detail
{
STREAMJSON_API inline FString Serialize(const UStruct* StructDefinition, const void* Struct, ENamingConvention NamingConvention);

template <class T>
T Deserialize(const FString& Json)
{
	T OutData;
	JsonObjectDeserialization::JsonObjectStringToUStruct<T>(Json, &OutData);
	return OutData;
}
}	 // namespace Detail
