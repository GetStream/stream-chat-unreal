#include "StreamJson.h"

#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

FString Json::Serialize(const TSharedRef<FJsonObject>& JsonObject)
{
    FString OutJsonString;
    const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJsonString);
    const bool bSuccess = FJsonSerializer::Serialize(JsonObject, JsonWriter);
    ensure(bSuccess);
    JsonWriter->Close();
    return OutJsonString;
}

void Json::SerializeField(const TOptional<uint32>& Field, const FString& FieldName, FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        JsonObject.SetNumberField(FieldName, Field.GetValue());
    }
}

void Json::DeserializeField(const FJsonObject& JsonObject, const FString& FieldName, TOptional<uint32>& Field)
{
    if (uint32 Number; JsonObject.TryGetNumberField(FieldName, Number))
    {
        Field.Emplace(Number);
    }
}
