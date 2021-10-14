#pragma once

#include "CoreMinimal.h"
#include "NamingConvention.h"

class FJsonObject;
class FJsonValue;

namespace JsonObjectSerialization
{

/**
 * Converts from a UStruct to a Json Object, using exportText
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param OutJsonObject Json Object to be filled in with data from the UStruct
 *
 * @return False if any properties failed to write
 */
STREAMJSON_API TSharedRef<FJsonObject> UStructToJsonObject(
    const UStruct* StructDefinition,
    const void* Struct,
    ENamingConvention NamingConvention = ENamingConvention::UpperCamelCase);

/**
 * Converts from a UStruct to a set of json attributes (possibly from within a JsonObject)
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param OutJsonAttributes Map of attributes to copy in to
 *
 * @return False if any properties failed to write
 */
bool UStructToJsonAttributes(
    const UStruct* StructDefinition,
    const void* Struct,
    TMap<FString, TSharedPtr<FJsonValue> >& OutJsonAttributes,
    ENamingConvention NamingConvention = ENamingConvention::UpperCamelCase);
}    // namespace JsonObjectSerialization
