#include "JsonObjectDeserialization.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "JsonObjectWrapper.h"
#include "NamingConvention.h"
#include "NamingConventionConversion.h"
#include "Policies/CondensedJsonPrintPolicy.h"

TSharedPtr<FJsonValue> ApplyNamingConventionToValue(FProperty* Property, const void* Value, ENamingConvention NamingConvention)
{
	if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		// export enums as strings
		const UEnum* EnumDef = EnumProperty->GetEnum();
		FString StringValue =
			EnumDef->GetNameStringByValue(EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(Value));
		if (NamingConvention == ENamingConvention::SnakeCase)
		{
			StringValue = NamingConventionConversion::ConvertNameToSnakeCase(StringValue);
		}

		return MakeShared<FJsonValueString>(StringValue);
	}
	return nullptr;
}

template <class CharType, class PrintPolicy>
bool UStructToJsonObjectStringInternal(const TSharedRef<FJsonObject>& JsonObject, FString& OutJsonString, int32 Indent)
{
	TSharedRef<TJsonWriter<CharType, PrintPolicy> > JsonWriter =
		TJsonWriterFactory<CharType, PrintPolicy>::Create(&OutJsonString, Indent);
	const bool bSuccess = FJsonSerializer::Serialize(JsonObject, JsonWriter);
	JsonWriter->Close();
	return bSuccess;
}

bool JsonObjectDeserialization::UStructToJsonObjectString(const UStruct* StructDefinition, const void* Struct,
	FString& OutJsonString, int64 CheckFlags, int64 SkipFlags, int32 Indent, ENamingConvention NamingConvention, bool bPrettyPrint)
{
	const TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	if (UStructToJsonObject(StructDefinition, Struct, JsonObject, CheckFlags, SkipFlags, NamingConvention))
	{
		bool bSuccess;
		if (bPrettyPrint)
		{
			bSuccess = UStructToJsonObjectStringInternal<TCHAR, TPrettyJsonPrintPolicy<TCHAR> >(JsonObject, OutJsonString, Indent);
		}
		else
		{
			bSuccess =
				UStructToJsonObjectStringInternal<TCHAR, TCondensedJsonPrintPolicy<TCHAR> >(JsonObject, OutJsonString, Indent);
		}
		if (bSuccess)
		{
			return true;
		}
		else
		{
			UE_LOG(LogJson, Warning, TEXT("UStructToJsonObjectString - Unable to write out json"));
		}
	}

	return false;
}

bool JsonObjectDeserialization::UStructToJsonObject(const UStruct* StructDefinition, const void* Struct,
	TSharedRef<FJsonObject> OutJsonObject, int64 CheckFlags, int64 SkipFlags, ENamingConvention NamingConvention)
{
	return UStructToJsonAttributes(StructDefinition, Struct, OutJsonObject->Values, CheckFlags, SkipFlags, NamingConvention);
}

bool JsonObjectDeserialization::UStructToJsonAttributes(const UStruct* StructDefinition, const void* Struct,
	TMap<FString, TSharedPtr<FJsonValue> >& OutJsonAttributes, int64 CheckFlags, int64 SkipFlags,
	ENamingConvention NamingConvention)
{
	if (SkipFlags == 0)
	{
		// If we have no specified skip flags, skip deprecated, transient and skip serialization by default when writing
		SkipFlags |= CPF_Deprecated | CPF_Transient;
	}

	if (StructDefinition == FJsonObjectWrapper::StaticStruct())
	{
		// Just copy it into the object
		const FJsonObjectWrapper* ProxyObject = static_cast<const FJsonObjectWrapper*>(Struct);

		if (ProxyObject->JsonObject.IsValid())
		{
			OutJsonAttributes = ProxyObject->JsonObject->Values;
		}
		return true;
	}

	for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
	{
		FProperty* Property = *It;

		// Check to see if we should ignore this property
		if (CheckFlags != 0 && !Property->HasAnyPropertyFlags(CheckFlags))
		{
			continue;
		}
		if (Property->HasAnyPropertyFlags(SkipFlags))
		{
			continue;
		}

		FString VariableName = Property->GetName();
		if (NamingConvention == ENamingConvention::SnakeCase)
		{
			VariableName = NamingConventionConversion::ConvertPropertyNameToSnakeCase(VariableName);
		}
		const void* Value = Property->ContainerPtrToValuePtr<uint8>(Struct);

		// convert the property to a FJsonValue
		FJsonObjectConverter::CustomExportCallback Callback =
			FJsonObjectConverter::CustomExportCallback::CreateStatic(&ApplyNamingConventionToValue, NamingConvention);
		TSharedPtr<FJsonValue> JsonValue =
			FJsonObjectConverter::UPropertyToJsonValue(Property, Value, CheckFlags, SkipFlags, &Callback);
		if (!JsonValue.IsValid())
		{
			const FFieldClass* PropClass = Property->GetClass();
			UE_LOG(LogJson, Error, TEXT("UStructToJsonObject - Unhandled property type '%s': %s"), *PropClass->GetName(),
				*Property->GetPathName());
			return false;
		}

		// set the value on the output object
		OutJsonAttributes.Add(VariableName, JsonValue);
	}

	return true;
}
