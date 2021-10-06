#include "Detail/JsonDetail.h"

#include "JsonObjectConverter.h"
#include "JsonObjectSerialization.h"

FString Detail::Serialize(const UStruct* StructDefinition, const void* Struct, ENamingConvention NamingConvention)
{
    FString Json;
    JsonObjectSerialization::UStructToJsonObjectString(StructDefinition, Struct, Json, NamingConvention, false);
    return Json;
}
