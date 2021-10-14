#include "Detail/JsonDetail.h"

#include "JsonObjectConverter.h"
#include "JsonObjectSerialization.h"

FString Detail::Serialize(const UStruct* Definition, const void* Struct, const ENamingConvention NamingConvention)
{
    FString Json;
    JsonObjectSerialization::UStructToJsonObjectString(Definition, Struct, Json, NamingConvention, false);
    return Json;
}
