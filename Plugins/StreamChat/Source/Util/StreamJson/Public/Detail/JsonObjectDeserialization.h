#pragma once

#include "CoreMinimal.h"
#include "ExtraFields.h"

class FJsonValue;
class FJsonObject;

/** Class that handles converting Json objects to UStructs */
namespace JsonObjectDeserialization
{
/**
 * Converts from a Json Object to a UStruct, using importText
 *
 * @param JsonObject Json Object to copy data out of
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param OutStruct The UStruct instance to copy in to
 *
 * @return False if any properties matched but failed to deserialize
 */
STREAMJSON_API bool JsonObjectToUStruct(
    const TSharedRef<FJsonObject>& JsonObject,
    const UStruct* StructDefinition,
    void* OutStruct);

/**
 * Templated version of JsonObjectToUStruct
 *
 * @param JsonObject Json Object to copy data out of
 * @param OutStruct The UStruct instance to copy in to
 *
 * @return False if any properties matched but failed to deserialize
 */
template <typename OutStructType>
bool JsonObjectToUStruct(const TSharedRef<FJsonObject>& JsonObject, OutStructType* OutStruct)
{
    const bool bSuccess = JsonObjectToUStruct(JsonObject, OutStructType::StaticStruct(), OutStruct);
    ExtraFields::InvokeDeserializeExtra<OutStructType>(*JsonObject, *OutStruct);
    return bSuccess;
}

/**
 * Converts a set of json attributes (possibly from within a JsonObject) to a UStruct, using importText
 *
 * @param JsonAttributes Json Object to copy data out of
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param OutStruct The UStruct instance to copy in to
 *
 * @return False if any properties matched but failed to deserialize
 */
bool JsonAttributesToUStruct(
    const TMap<FString, TSharedPtr<FJsonValue>>& JsonAttributes,
    const UStruct* StructDefinition,
    void* OutStruct);

/**
 * Converts from a json string containing an object to a UStruct
 *
 * @param JsonString String containing JSON formatted data.
 * @param OutObject JsonObject to created from parsed JSON text
 *
 * @return False if any properties matched but failed to deserialize
 */
STREAMJSON_API bool JsonObjectStringToJsonObject(const FString& JsonString, TSharedPtr<FJsonObject>& OutObject);

/**
 * Converts from a json string containing an object to a UStruct
 *
 * @param JsonString String containing JSON formatted data.
 * @param OutStruct The UStruct instance to copy in to
 *
 * @return False if any properties matched but failed to deserialize
 */
template <typename OutStructType>
bool JsonObjectStringToUStruct(const FString& JsonString, OutStructType* OutStruct)
{
    TSharedPtr<FJsonObject> JsonObject;
    if (!JsonObjectDeserialization::JsonObjectStringToJsonObject(JsonString, JsonObject))
    {
        UE_LOG(LogTemp, Warning, TEXT("JsonObjectStringToUStruct - Unable to parse json=[%s]"), *JsonString);
        return false;
    }
    if (!JsonObjectDeserialization::JsonObjectToUStruct(JsonObject.ToSharedRef(), OutStruct))
    {
        UE_LOG(LogTemp, Warning, TEXT("JsonObjectStringToUStruct - Unable to deserialize. json=[%s]"), *JsonString);
        return false;
    }
    return true;
}
};    // namespace JsonObjectDeserialization
