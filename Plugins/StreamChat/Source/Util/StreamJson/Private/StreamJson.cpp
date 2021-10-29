#include "StreamJson.h"

#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"

TSharedRef<FJsonObject> Json::UStructToJsonObject(
    const UStruct* StructDefinition,
    const void* Struct,
    const ENamingConvention NamingConvention)
{
    const TSharedRef<FJsonObject> OutJsonObject = MakeShared<FJsonObject>();
    JsonObjectSerialization::UStructToJsonAttributes(StructDefinition, Struct, OutJsonObject->Values, NamingConvention);
    return OutJsonObject;
}

FString Json::JsonObjectToString(const TSharedRef<FJsonObject>& JsonObject)
{
    FString OutJsonString;
    const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJsonString);
    const bool bSuccess = FJsonSerializer::Serialize(JsonObject, JsonWriter);
    ensure(bSuccess);
    JsonWriter->Close();
    return OutJsonString;
}

template <>
void Json::SerializeField<uint32>(const TOptional<uint32>& Field, const FString& FieldName, FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        JsonObject.SetNumberField(FieldName, Field.GetValue());
    }
}

template <>
void Json::SerializeField<FString>(const TOptional<FString>& Field, const FString& FieldName, FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        JsonObject.SetStringField(FieldName, Field.GetValue());
    }
}

template <>
void Json::SerializeField<FDateTime>(
    const TOptional<FDateTime>& Field,
    const FString& FieldName,
    FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        JsonObject.SetStringField(FieldName, Field.GetValue().ToIso8601());
    }
}

template <>
void Json::DeserializeField<uint32>(const FJsonObject& JsonObject, const FString& FieldName, TOptional<uint32>& Field)
{
    if (uint32 Number; JsonObject.TryGetNumberField(FieldName, Number))
    {
        Field.Emplace(Number);
    }
}

template <>
void Json::DeserializeField<FDateTime>(
    const FJsonObject& JsonObject,
    const FString& FieldName,
    TOptional<FDateTime>& Field)
{
    if (FString DateString; JsonObject.TryGetStringField(FieldName, DateString))
    {
        if (FDateTime Date; FDateTime::ParseIso8601(*DateString, Date))
        {
            Field.Emplace(Date);
        }
    }
}
