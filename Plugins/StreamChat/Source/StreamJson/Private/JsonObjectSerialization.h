#pragma once
#include "NamingConvention.h"

class FJsonObject;
class FJsonValue;

namespace JsonObjectSerialization
{
/**
 * Converts from a UStruct to a json string containing an object, using exportText
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param OutJsonString Json string to be filled in with data from the UStruct
 * @param bPrettyPrint Option to use pretty print (e.g., adds line endings) or condensed print
 *
 * @return False if any properties failed to write
 */
static bool UStructToJsonObjectString(const UStruct* StructDefinition, const void* Struct, FString& OutJsonString,
	ENamingConvention NamingConvention = ENamingConvention::UpperCamelCase, bool bPrettyPrint = true);

/**
 * Converts from a UStruct to a Json Object, using exportText
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param OutJsonObject Json Object to be filled in with data from the UStruct
 *
 * @return False if any properties failed to write
 */
static bool UStructToJsonObject(const UStruct* StructDefinition, const void* Struct, TSharedRef<FJsonObject> OutJsonObject,
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
static bool UStructToJsonAttributes(const UStruct* StructDefinition, const void* Struct,
	TMap<FString, TSharedPtr<FJsonValue> >& OutJsonAttributes,
	ENamingConvention NamingConvention = ENamingConvention::UpperCamelCase);
}	 // namespace JsonObjectSerialization
