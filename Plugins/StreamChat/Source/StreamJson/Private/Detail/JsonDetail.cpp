#include "Detail/JsonDetail.h"

#include "JsonObjectConverter.h"
#include "JsonObjectDeserialization.h"

FString Detail::Serialize(const UStruct* StructDefinition, const void* Struct, ENamingConvention NamingConvention)
{
	FString Json;
	JsonObjectDeserialization::UStructToJsonObjectString(StructDefinition, Struct, Json, NamingConvention, false);
	return Json;
}
