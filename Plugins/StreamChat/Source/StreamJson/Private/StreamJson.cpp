#include "StreamJson.h"

#include "Serialization/JsonSerializer.h"

FString Json::Serialize(const TSharedRef<FJsonObject>& JsonObject)
{
    return Detail::SerializeJsonObject(JsonObject);
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
