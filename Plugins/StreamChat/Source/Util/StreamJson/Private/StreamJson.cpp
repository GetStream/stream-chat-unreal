// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamJson.h"

#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"

TSharedRef<FJsonObject> JsonObject::UStructToJsonObject(
    const UStruct* StructDefinition,
    const void* Struct,
    const ENamingConvention NamingConvention)
{
    const TSharedRef<FJsonObject> OutJsonObject = MakeShared<FJsonObject>();
    JsonObjectSerialization::UStructToJsonAttributes(StructDefinition, Struct, OutJsonObject->Values, NamingConvention);
    return OutJsonObject;
}

FString JsonObject::JsonObjectToString(const TSharedRef<FJsonObject>& JsonObject)
{
    FString OutJsonString;
    const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJsonString);
    const bool bSuccess = FJsonSerializer::Serialize(JsonObject, JsonWriter);
    ensure(bSuccess);
    JsonWriter->Close();
    return OutJsonString;
}
