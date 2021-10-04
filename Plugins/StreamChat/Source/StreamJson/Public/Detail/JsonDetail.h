#pragma once

#include "JsonObjectConverter.h"

namespace Detail
{
STREAMJSON_API inline FString Serialize(const UStruct* StructDefinition, const void* Struct)
{
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(StructDefinition, Struct, Json, 0, 0, 0, nullptr, false);
	return Json;
}

template <class T>
T Deserialize(const FString& Json)
{
	T OutData;
	FJsonObjectConverter::JsonObjectStringToUStruct<T>(Json, &OutData);
	return OutData;
}
}	 // namespace Detail
