#pragma once

#include "CoreMinimal.h"
#include "NamingConvention.h"

class FJsonObject;
class FJsonValue;

namespace JsonObjectSerialization
{
/**
 * Converts from a UStruct to a set of json attributes (possibly from within a JsonObject)
 *
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param Struct The UStruct instance to copy out of
 * @param OutJsonAttributes Map of attributes to copy in to
 * @param NamingConvention JSON keys and values will be formatted according to this naming convention
 *
 * @return False if any properties failed to write
 */
bool UStructToJsonAttributes(
    const UStruct* StructDefinition,
    const void* Struct,
    TMap<FString, TSharedPtr<FJsonValue> >& OutJsonAttributes,
    ENamingConvention NamingConvention = ENamingConvention::SnakeCase);

FString STREAMJSON_API UEnumToString(
    const UEnum* EnumDefinition,
    int64 Value,
    ENamingConvention NamingConvention = ENamingConvention::SnakeCase);

void STREAMJSON_API
SetObjectField(FJsonObject& TargetJsonObject, const FString& FieldName, const TSharedPtr<FJsonObject>& FieldJsonObject);
bool STREAMJSON_API TryGetObjectField(
    const FJsonObject& TargetJsonObject,
    const FString& FieldName,
    const TSharedPtr<FJsonObject>*& FieldJsonObject);
}    // namespace JsonObjectSerialization
