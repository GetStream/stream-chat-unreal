#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "JsonObjectWrapper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

/** Class that handles converting Json objects to UStructs */
namespace JsonObjectSerialization
{
/**
 * Converts from a Json Object to a UStruct, using importText
 *
 * @param JsonObject Json Object to copy data out of
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param OutStruct The UStruct instance to copy in to
 * @param CheckFlags Only convert properties that match at least one of these flags. If 0 check all properties.
 * @param SkipFlags Skip properties that match any of these flags
 *
 * @return False if any properties matched but failed to deserialize
 */
STREAMJSON_API bool JsonObjectToUStruct(const TSharedRef<FJsonObject>& JsonObject, const UStruct* StructDefinition, void* OutStruct,
	int64 CheckFlags = 0, int64 SkipFlags = 0);

/**
 * Templated version of JsonObjectToUStruct
 *
 * @param JsonObject Json Object to copy data out of
 * @param OutStruct The UStruct instance to copy in to
 * @param CheckFlags Only convert properties that match at least one of these flags. If 0 check all properties.
 * @param SkipFlags Skip properties that match any of these flags
 *
 * @return False if any properties matched but failed to deserialize
 */
template <typename OutStructType>
bool JsonObjectToUStruct(
	const TSharedRef<FJsonObject>& JsonObject, OutStructType* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0)
{
	return JsonObjectToUStruct(JsonObject, OutStructType::StaticStruct(), OutStruct, CheckFlags, SkipFlags);
}

/**
 * Converts a set of json attributes (possibly from within a JsonObject) to a UStruct, using importText
 *
 * @param JsonAttributes Json Object to copy data out of
 * @param StructDefinition UStruct definition that is looked over for properties
 * @param OutStruct The UStruct instance to copy in to
 * @param CheckFlags Only convert properties that match at least one of these flags. If 0 check all properties.
 * @param SkipFlags Skip properties that match any of these flags
 *
 * @return False if any properties matched but failed to deserialize
 */
bool JsonAttributesToUStruct(const TMap<FString, TSharedPtr<FJsonValue>>& JsonAttributes, const UStruct* StructDefinition,
	void* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0);

/**
 * Converts from a json string containing an object to a UStruct
 *
 * @param JsonString String containing JSON formatted data.
 * @param OutStruct The UStruct instance to copy in to
 * @param CheckFlags Only convert properties that match at least one of these flags. If 0 check all properties.
 * @param SkipFlags Skip properties that match any of these flags
 *
 * @return False if any properties matched but failed to deserialize
 */
template <typename OutStructType>
bool JsonObjectStringToUStruct(const FString& JsonString, OutStructType* OutStruct, int64 CheckFlags = 0, int64 SkipFlags = 0)
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogJson, Warning, TEXT("JsonObjectStringToUStruct - Unable to parse json=[%s]"), *JsonString);
		return false;
	}
	if (!JsonObjectSerialization::JsonObjectToUStruct(JsonObject.ToSharedRef(), OutStruct, CheckFlags, SkipFlags))
	{
		UE_LOG(LogJson, Warning, TEXT("JsonObjectStringToUStruct - Unable to deserialize. json=[%s]"), *JsonString);
		return false;
	}
	return true;
}
};	  // namespace JsonObjectSerialization
