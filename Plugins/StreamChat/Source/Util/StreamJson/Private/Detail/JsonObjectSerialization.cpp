// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Detail/JsonObjectSerialization.h"

#include "AdditionalFields.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "JsonObjectWrapper.h"
#include "NamingConvention.h"
#include "NamingConventionConversion.h"
#include "StreamJson.h"

namespace
{
TSharedPtr<FJsonValue> ApplyNamingConventionToValue(FProperty* Property, const void* Value, const ENamingConvention NamingConvention)
{
    if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
    {
        // export enums as strings
        const UEnum* EnumDef = EnumProperty->GetEnum();
        const FString StringValue =
            JsonObjectSerialization::UEnumToString(EnumDef, EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(Value), NamingConvention);

        return MakeShared<FJsonValueString>(StringValue);
    }
    if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
    {
        static const FName NAME_DateTime(TEXT("DateTime"));

        // DateTimes should always be converted to ISO 8601 strings
        if (StructProperty->Struct->GetFName() == NAME_DateTime)
        {
            const FDateTime* DateTime = static_cast<const FDateTime*>(Value);
            return MakeShared<FJsonValueString>(DateTime->ToIso8601());
        }
        // Intentionally exclude the JSON Object wrapper, which specifically needs to export JSON in an object
        // representation instead of a string
        UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();
        if (StructProperty->Struct != FJsonObjectWrapper::StaticStruct() && TheCppStructOps && TheCppStructOps->HasExportTextItem())
        {
            FString OutValueStr;
            TheCppStructOps->ExportTextItem(OutValueStr, Value, nullptr, nullptr, PPF_None, nullptr);
            return MakeShared<FJsonValueString>(OutValueStr);
        }

        TSharedRef<FJsonObject> Out = JsonObject::UStructToJsonObject(StructProperty->Struct, Value, NamingConvention);
        return MakeShared<FJsonValueObject>(Out);
    }
    return nullptr;
}
FString ApplyNamingConventionToPropertyName(const FString& PropertyName, ENamingConvention NamingConvention)
{
    if (NamingConvention == ENamingConvention::UpperCamelCase)
    {
        return NamingConventionConversion::ConvertPropertyNameToUpperCamelCase(PropertyName);
    }
    if (NamingConvention == ENamingConvention::SnakeCase)
    {
        return NamingConventionConversion::ConvertPropertyNameToSnakeCase(PropertyName);
    }
    return PropertyName;
}
}    // namespace

template <class CharType, class PrintPolicy>
bool UStructToJsonObjectStringInternal(const TSharedRef<FJsonObject>& JsonObject, FString& OutJsonString)
{
    TSharedRef<TJsonWriter<CharType, PrintPolicy>> JsonWriter = TJsonWriterFactory<CharType, PrintPolicy>::Create(&OutJsonString);
    const bool bSuccess = FJsonSerializer::Serialize(JsonObject, JsonWriter);
    JsonWriter->Close();
    return bSuccess;
}

bool JsonObjectSerialization::UStructToJsonAttributes(
    const UStruct* StructDefinition,
    const void* Struct,
    TMap<FString, TSharedPtr<FJsonValue>>& OutJsonAttributes,
    const ENamingConvention NamingConvention)
{
    // Skip deprecated, transient and skip serialization by default when writing
    constexpr int64 SkipFlags = CPF_Deprecated | CPF_Transient;

    if (StructDefinition == FJsonObjectWrapper::StaticStruct())
    {
        const FJsonObjectWrapper* ProxyObject = static_cast<const FJsonObjectWrapper*>(Struct);
        if (ProxyObject->JsonObject.IsValid())
        {
            OutJsonAttributes = ProxyObject->JsonObject->Values;
        }
        return true;
    }

    const auto Callback = FJsonObjectConverter::CustomExportCallback::CreateStatic(&ApplyNamingConventionToValue, NamingConvention);

    for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
    {
        FProperty* Property = *It;

        // Check to see if we should ignore this property
        if (Property->HasAnyPropertyFlags(SkipFlags))
        {
            continue;
        }

        // Flatten additional fields into outer struct
        if (const FAdditionalFields* Fields = FAdditionalFields::FromProperty(Struct, Property))
        {
            for (auto&& Elem : Fields->GetFields())
            {
                const FString Key = ApplyNamingConventionToPropertyName(Elem.Key.ToString(), NamingConvention);
                OutJsonAttributes.Add(Key, Elem.Value);
            }
            continue;
        }

        const void* Value = Property->ContainerPtrToValuePtr<uint8>(Struct);

        // Skip serializing empty arrays
        if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
        {
            const FScriptArrayHelper ArrayHelper{ArrayProperty, Value};
            if (ArrayHelper.Num() == 0)
            {
                continue;
            }
        }

        // Skip serializing empty maps
        if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
        {
            const FScriptMapHelper MapHelper{MapProperty, Value};
            if (MapHelper.Num() == 0)
            {
                continue;
            }
        }

        // Skip serializing empty strings
        if (const FStrProperty* StrProperty = CastField<FStrProperty>(Property))
        {
            if (StrProperty->GetPropertyValue(Value).IsEmpty())
            {
                continue;
            }
        }

        // Skip serializing zeroed date/times
        if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
        {
            static const FName NAME_DateTime(TEXT("DateTime"));
            if (StructProperty->Struct->GetFName() == NAME_DateTime)
            {
                const FDateTime* DateTime = static_cast<const FDateTime*>(Value);
                if (DateTime->GetTicks() == 0)
                {
                    continue;
                }
            }
        }

        // convert the property to a FJsonValue
        TSharedPtr<FJsonValue> JsonValue = FJsonObjectConverter::UPropertyToJsonValue(Property, Value, 0, SkipFlags, &Callback);
        if (!JsonValue.IsValid())
        {
            const FFieldClass* PropClass = Property->GetClass();
            UE_LOG(LogJson, Error, TEXT("UStructToJsonObject - Unhandled property type '%s': %s"), *PropClass->GetName(), *Property->GetPathName());
            return false;
        }

        // set the value on the output object
        const FString VariableName = ApplyNamingConventionToPropertyName(Property->GetName(), NamingConvention);
        OutJsonAttributes.Add(VariableName, JsonValue);
    }

    return true;
}

FString JsonObjectSerialization::UEnumToString(const UEnum* EnumDefinition, const int64 Value, const ENamingConvention NamingConvention)
{
    FString StringValue = EnumDefinition->GetNameStringByValue(Value);
    if (NamingConvention == ENamingConvention::SnakeCase)
    {
        StringValue = NamingConventionConversion::ConvertNameToSnakeCase(StringValue);
    }
    return StringValue;
}

void JsonObjectSerialization::SetObjectField(FJsonObject& TargetJsonObject, const FString& FieldName, const TSharedPtr<FJsonObject>& FieldJsonObject)
{
    TargetJsonObject.SetObjectField(FieldName, FieldJsonObject);
}

bool JsonObjectSerialization::TryGetObjectField(const FJsonObject& TargetJsonObject, const FString& FieldName, const TSharedPtr<FJsonObject>*& FieldJsonObject)
{
    return TargetJsonObject.TryGetObjectField(FieldName, FieldJsonObject);
}
