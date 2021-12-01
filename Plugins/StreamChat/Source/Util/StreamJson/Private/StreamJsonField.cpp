// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "StreamJsonField.h"

#include "Serialization/JsonSerializer.h"

template <>
void JsonField::Serialize<uint32>(const TOptional<uint32>& Field, const FString& FieldName, FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        JsonObject.SetNumberField(FieldName, Field.GetValue());
    }
}

template <>
void JsonField::Serialize<FString>(const TOptional<FString>& Field, const FString& FieldName, FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        JsonObject.SetStringField(FieldName, Field.GetValue());
    }
}

template <>
void JsonField::Serialize<FDateTime>(const TOptional<FDateTime>& Field, const FString& FieldName, FJsonObject& JsonObject)
{
    if (Field.IsSet())
    {
        JsonObject.SetStringField(FieldName, Field.GetValue().ToIso8601());
    }
}

template <>
void JsonField::Deserialize<uint32>(const FJsonObject& JsonObject, const FString& FieldName, TOptional<uint32>& Field)
{
    if (uint32 Number; JsonObject.TryGetNumberField(FieldName, Number))
    {
        Field.Emplace(Number);
    }
}

template <>
void JsonField::Deserialize<FDateTime>(const FJsonObject& JsonObject, const FString& FieldName, TOptional<FDateTime>& Field)
{
    if (FString DateString; JsonObject.TryGetStringField(FieldName, DateString))
    {
        if (FDateTime Date; FDateTime::ParseIso8601(*DateString, Date))
        {
            Field.Emplace(Date);
        }
    }
}
