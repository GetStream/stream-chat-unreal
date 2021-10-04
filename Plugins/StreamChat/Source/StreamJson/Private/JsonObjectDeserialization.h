#pragma once
#include "NamingConvention.h"

class FJsonObject;
class FJsonValue;

namespace JsonObjectDeserialization
{
/**
 * Converts from a UStruct to a json string containing an object, using exportText
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param JsonObject Json Object to be filled in with data from the ustruct
 * @param CheckFlags Only convert properties that match at least one of these flags. If 0 check all properties.
 * @param SkipFlags Skip properties that match any of these flags
 * @param Indent How many tabs to add to the json serializer
 * @param ExportCb Optional callback to override export behavior, if this returns null it will fallback to the default
 * @param bPrettyPrint Option to use pretty print (e.g., adds line endings) or condensed print
 *
 * @return False if any properties failed to write
 */
static bool UStructToJsonObjectString(const UStruct* StructDefinition, const void* Struct, FString& OutJsonString,
	int64 CheckFlags = 0, int64 SkipFlags = 0, int32 Indent = 0,
	ENamingConvention NamingConvention = ENamingConvention::UpperCamelCase, bool bPrettyPrint = true);

/**
 * Converts from a UStruct to a Json Object, using exportText
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param JsonObject Json Object to be filled in with data from the ustruct
 * @param CheckFlags Only convert properties that match at least one of these flags. If 0 check all properties.
 * @param SkipFlags Skip properties that match any of these flags
 * @param ExportCb Optional callback to override export behavior, if this returns null it will fallback to the default
 *
 * @return False if any properties failed to write
 */
static bool UStructToJsonObject(const UStruct* StructDefinition, const void* Struct, TSharedRef<FJsonObject> OutJsonObject,
	int64 CheckFlags = 0, int64 SkipFlags = 0, ENamingConvention NamingConvention = ENamingConvention::UpperCamelCase);

/**
 * Converts from a UStruct to a set of json attributes (possibly from within a JsonObject)
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param JsonAttributes Map of attributes to copy in to
 * @param CheckFlags Only convert properties that match at least one of these flags. If 0 check all properties.
 * @param SkipFlags Skip properties that match any of these flags
 * @param ExportCb Optional callback to override export behavior, if this returns null it will fallback to the default
 *
 * @return False if any properties failed to write
 */
static bool UStructToJsonAttributes(const UStruct* StructDefinition, const void* Struct,
	TMap<FString, TSharedPtr<FJsonValue> >& OutJsonAttributes, int64 CheckFlags = 0, int64 SkipFlags = 0,
	ENamingConvention NamingConvention = ENamingConvention::UpperCamelCase);
}	 // namespace JsonObjectDeserialization
