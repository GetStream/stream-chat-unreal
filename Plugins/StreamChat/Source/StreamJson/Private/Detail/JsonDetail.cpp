#include "Detail/JsonDetail.h"

#include "JsonObjectConverter.h"
#include "NamingConventionConversion.h"

FString Detail::Serialize(const UStruct* StructDefinition, const void* Struct, ENamingConvention NamingConvention)
{
	FString Json;
	const FJsonObjectConverter::CustomExportCallback Callback = FJsonObjectConverter::CustomExportCallback::CreateLambda(
		[NamingConvention](FProperty* Property, const void* Value) -> TSharedPtr<FJsonValue>
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
		});
	FJsonObjectConverter::UStructToJsonObjectString(StructDefinition, Struct, Json, 0, 0, 0, &Callback, false);

	return Json;
}
