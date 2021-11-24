// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Detail/JsonObjectDeserialization.h"

#include "Internationalization/Culture.h"
#include "JsonObjectWrapper.h"
#include "NamingConventionConversion.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/Class.h"
#include "UObject/EnumProperty.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Package.h"
#include "UObject/TextProperty.h"
#include "UObject/UnrealType.h"

namespace
{
const FString ObjectClassNameKey = "_ClassName";

/** Parse an FText from a json object (assumed to be of the form where keys are culture codes and values are strings) */
bool GetTextFromObject(const TSharedRef<FJsonObject>& Obj, FText& TextOut)
{
    // get the prioritized culture name list
    const FCultureRef CurrentCulture = FInternationalization::Get().GetCurrentCulture();
    TArray<FString> CultureList = CurrentCulture->GetPrioritizedParentCultureNames();

    // try to follow the fall back chain that the engine uses
    FString TextString;
    for (const FString& CultureCode : CultureList)
    {
        if (Obj->TryGetStringField(CultureCode, TextString))
        {
            TextOut = FText::FromString(TextString);
            return true;
        }
    }

    // try again but only search on the locale region (in the localized data). This is a common omission (i.e. en-US
    // source text should be used if no en is defined)
    for (const FString& LocaleToMatch : CultureList)
    {
        int32 SeparatorPos;
        // only consider base language entries in culture chain (i.e. "en")
        if (!LocaleToMatch.FindChar('-', SeparatorPos))
        {
            for (const auto& Pair : Obj->Values)
            {
                // only consider coupled entries now (base ones would have been matched on first path) (i.e. "en-US")
                if (Pair.Key.FindChar('-', SeparatorPos))
                {
                    if (Pair.Key.StartsWith(LocaleToMatch))
                    {
                        TextOut = FText::FromString(Pair.Value->AsString());
                        return true;
                    }
                }
            }
        }
    }

    // no luck, is this possibly an unrelated json object?
    return false;
}

bool JsonValueToFPropertyWithContainer(
    const TSharedPtr<FJsonValue>& JsonValue,
    FProperty* Property,
    void* OutValue,
    const UStruct* ContainerStruct,
    void* Container);

bool JsonAttributesToUStructWithContainer(
    const TMap<FString, TSharedPtr<FJsonValue>>& JsonAttributes,
    const UStruct* StructDefinition,
    void* OutStruct,
    const UStruct* ContainerStruct,
    void* Container);

/** Convert JSON to property, assuming either the property is not an array or the value is an individual array element
 */
bool ConvertScalarJsonValueToFPropertyWithContainer(
    const TSharedPtr<FJsonValue>& JsonValue,
    FProperty* Property,
    void* OutValue,
    const UStruct* ContainerStruct,
    void* Container)
{
    if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
    {
        if (JsonValue->Type == EJson::String)
        {
            // see if we were passed a string for the enum
            const UEnum* Enum = EnumProperty->GetEnum();
            check(Enum);
            FString StrValue = JsonValue->AsString();
            int64 IntValue = Enum->GetValueByName(FName(*StrValue));

            // Try converting from snake_case to UpperCamelCase
            if (IntValue == INDEX_NONE)
            {
                StrValue = NamingConventionConversion::ConvertSnakeCaseToUpperCamelCase(StrValue);
                IntValue = Enum->GetValueByName(FName(*StrValue));
            }

            if (IntValue == INDEX_NONE)
            {
                UE_LOG(
                    LogJson,
                    Error,
                    TEXT("JsonValueToUProperty - Unable import enum %s from string value %s for property %s"),
                    *Enum->CppType,
                    *StrValue,
                    *Property->GetNameCPP());
                return false;
            }
            EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutValue, IntValue);
        }
        else
        {
            // AsNumber will log an error for completely inappropriate types (then give us a default)
            EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutValue, static_cast<int64>(JsonValue->AsNumber()));
        }
    }
    else if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
    {
        if (NumericProperty->IsEnum() && JsonValue->Type == EJson::String)
        {
            // see if we were passed a string for the enum
            const UEnum* Enum = NumericProperty->GetIntPropertyEnum();
            check(Enum);    // should be assured by IsEnum()
            FString StrValue = JsonValue->AsString();
            int64 IntValue = Enum->GetValueByName(FName(*StrValue));
            if (IntValue == INDEX_NONE)
            {
                UE_LOG(
                    LogJson,
                    Error,
                    TEXT("JsonValueToUProperty - Unable import enum %s from string value %s for property %s"),
                    *Enum->CppType,
                    *StrValue,
                    *Property->GetNameCPP());
                return false;
            }
            NumericProperty->SetIntPropertyValue(OutValue, IntValue);
        }
        else if (NumericProperty->IsFloatingPoint())
        {
            // AsNumber will log an error for completely inappropriate types (then give us a default)
            NumericProperty->SetFloatingPointPropertyValue(OutValue, JsonValue->AsNumber());
        }
        else if (NumericProperty->IsInteger())
        {
            if (JsonValue->Type == EJson::String)
            {
                // parse string -> int64 ourselves so we don't lose any precision going through AsNumber (aka double)
                NumericProperty->SetIntPropertyValue(OutValue, FCString::Atoi64(*JsonValue->AsString()));
            }
            else
            {
                // AsNumber will log an error for completely inappropriate types (then give us a default)
                NumericProperty->SetIntPropertyValue(OutValue, static_cast<int64>(JsonValue->AsNumber()));
            }
        }
        else
        {
            UE_LOG(
                LogJson,
                Error,
                TEXT("JsonValueToUProperty - Unable to set numeric property type %s for property %s"),
                *Property->GetClass()->GetName(),
                *Property->GetNameCPP());
            return false;
        }
    }
    else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
    {
        // AsBool will log an error for completely inappropriate types (then give us a default)
        BoolProperty->SetPropertyValue(OutValue, JsonValue->AsBool());
    }
    else if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
    {
        // AsString will log an error for completely inappropriate types (then give us a default)
        StringProperty->SetPropertyValue(OutValue, JsonValue->AsString());
    }
    else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
    {
        if (JsonValue->Type == EJson::Array)
        {
            TArray<TSharedPtr<FJsonValue>> ArrayValue = JsonValue->AsArray();
            int32 ArrLen = ArrayValue.Num();

            // make the output array size match
            FScriptArrayHelper Helper(ArrayProperty, OutValue);
            Helper.Resize(ArrLen);

            // set the property values
            for (int32 i = 0; i < ArrLen; ++i)
            {
                const TSharedPtr<FJsonValue>& ArrayValueItem = ArrayValue[i];
                if (ArrayValueItem.IsValid() && !ArrayValueItem->IsNull())
                {
                    if (!JsonValueToFPropertyWithContainer(ArrayValueItem, ArrayProperty->Inner, Helper.GetRawPtr(i), ContainerStruct, Container))
                    {
                        UE_LOG(
                            LogJson,
                            Error,
                            TEXT("JsonValueToUProperty - Unable to deserialize array element [%d] for property %s"),
                            i,
                            *Property->GetNameCPP());
                        return false;
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import TArray from non-array JSON key for property %s"), *Property->GetNameCPP());
            return false;
        }
    }
    else if (FMapProperty* MapProperty = CastField<FMapProperty>(Property))
    {
        if (JsonValue->Type == EJson::Object)
        {
            TSharedPtr<FJsonObject> ObjectValue = JsonValue->AsObject();

            FScriptMapHelper Helper(MapProperty, OutValue);

            check(ObjectValue);

            int32 MapSize = ObjectValue->Values.Num();
            Helper.EmptyValues(MapSize);

            // set the property values
            for (const auto& Entry : ObjectValue->Values)
            {
                if (Entry.Value.IsValid() && !Entry.Value->IsNull())
                {
                    int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();

                    TSharedPtr<FJsonValueString> TempKeyValue = MakeShared<FJsonValueString>(Entry.Key);

                    const bool bKeySuccess =
                        JsonValueToFPropertyWithContainer(TempKeyValue, MapProperty->KeyProp, Helper.GetKeyPtr(NewIndex), ContainerStruct, Container);
                    const bool bValueSuccess =
                        JsonValueToFPropertyWithContainer(Entry.Value, MapProperty->ValueProp, Helper.GetValuePtr(NewIndex), ContainerStruct, Container);

                    if (!(bKeySuccess && bValueSuccess))
                    {
                        UE_LOG(
                            LogJson,
                            Error,
                            TEXT("JsonValueToUProperty - Unable to deserialize map element [key: %s] for property %s"),
                            *Entry.Key,
                            *Property->GetNameCPP());
                        return false;
                    }
                }
            }

            Helper.Rehash();
        }
        else
        {
            UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import TMap from non-object JSON key for property %s"), *Property->GetNameCPP());
            return false;
        }
    }
    else if (FSetProperty* SetProperty = CastField<FSetProperty>(Property))
    {
        if (JsonValue->Type == EJson::Array)
        {
            TArray<TSharedPtr<FJsonValue>> ArrayValue = JsonValue->AsArray();
            int32 ArrLen = ArrayValue.Num();

            FScriptSetHelper Helper(SetProperty, OutValue);

            // set the property values
            for (int32 i = 0; i < ArrLen; ++i)
            {
                const TSharedPtr<FJsonValue>& ArrayValueItem = ArrayValue[i];
                if (ArrayValueItem.IsValid() && !ArrayValueItem->IsNull())
                {
                    int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();
                    if (!JsonValueToFPropertyWithContainer(
                            ArrayValueItem, SetProperty->ElementProp, Helper.GetElementPtr(NewIndex), ContainerStruct, Container))
                    {
                        UE_LOG(
                            LogJson, Error, TEXT("JsonValueToUProperty - Unable to deserialize set element [%d] for property %s"), i, *Property->GetNameCPP());
                        return false;
                    }
                }
            }

            Helper.Rehash();
        }
        else
        {
            UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import TSet from non-array JSON key for property %s"), *Property->GetNameCPP());
            return false;
        }
    }
    else if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
    {
        if (JsonValue->Type == EJson::String)
        {
            // assume this string is already localized, so import as invariant
            TextProperty->SetPropertyValue(OutValue, FText::FromString(JsonValue->AsString()));
        }
        else if (JsonValue->Type == EJson::Object)
        {
            TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();
            check(Obj.IsValid());    // should not fail if Type == EJson::Object

            // import the subvalue as a culture invariant string
            FText Text;
            if (!GetTextFromObject(Obj.ToSharedRef(), Text))
            {
                UE_LOG(
                    LogJson,
                    Error,
                    TEXT("JsonValueToUProperty - Attempted to import FText from JSON object with invalid keys for "
                         "property %s"),
                    *Property->GetNameCPP());
                return false;
            }
            TextProperty->SetPropertyValue(OutValue, Text);
        }
        else
        {
            UE_LOG(
                LogJson,
                Error,
                TEXT("JsonValueToUProperty - Attempted to import FText from JSON that was neither string nor object "
                     "for property "
                     "%s"),
                *Property->GetNameCPP());
            return false;
        }
    }
    else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
    {
        static const FName NAME_DateTime(TEXT("DateTime"));
        static const FName NAME_Color(TEXT("Color"));
        static const FName NAME_LinearColor(TEXT("LinearColor"));
        if (JsonValue->Type == EJson::Object)
        {
            TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();
            check(Obj.IsValid());    // should not fail if Type == EJson::Object
            if (!JsonAttributesToUStructWithContainer(Obj->Values, StructProperty->Struct, OutValue, ContainerStruct, Container))
            {
                UE_LOG(
                    LogJson,
                    Error,
                    TEXT("JsonValueToUProperty - JsonObjectDeserialization::JsonObjectToUStruct failed for property %s"),
                    *Property->GetNameCPP());
                return false;
            }
        }
        else if (JsonValue->Type == EJson::String && StructProperty->Struct->GetFName() == NAME_LinearColor)
        {
            FLinearColor& ColorOut = *static_cast<FLinearColor*>(OutValue);
            FString ColorString = JsonValue->AsString();

            FColor IntermediateColor;
            IntermediateColor = FColor::FromHex(ColorString);

            ColorOut = IntermediateColor;
        }
        else if (JsonValue->Type == EJson::String && StructProperty->Struct->GetFName() == NAME_Color)
        {
            FColor& ColorOut = *static_cast<FColor*>(OutValue);
            FString ColorString = JsonValue->AsString();

            ColorOut = FColor::FromHex(ColorString);
        }
        else if (JsonValue->Type == EJson::String && StructProperty->Struct->GetFName() == NAME_DateTime)
        {
            FString DateString = JsonValue->AsString();
            FDateTime& DateTimeOut = *static_cast<FDateTime*>(OutValue);
            if (DateString == TEXT("min"))
            {
                // min representable value for our date struct. Actual date may vary by platform (this is used for
                // sorting)
                DateTimeOut = FDateTime::MinValue();
            }
            else if (DateString == TEXT("max"))
            {
                // max representable value for our date struct. Actual date may vary by platform (this is used for
                // sorting)
                DateTimeOut = FDateTime::MaxValue();
            }
            else if (DateString == TEXT("now"))
            {
                // this value's not really meaningful from json serialization (since we don't know timezone) but handle
                // it anyway since we're handling the other keywords
                DateTimeOut = FDateTime::UtcNow();
            }
            else if (FDateTime::ParseIso8601(*DateString, DateTimeOut))
            {
                // ok
            }
            else if (FDateTime::Parse(DateString, DateTimeOut))
            {
                // ok
            }
            else
            {
                UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable to import FDateTime [Property=%s, Value=%s]"), *Property->GetNameCPP(), *DateString);
                return false;
            }
        }
        else if (
            JsonValue->Type == EJson::String && StructProperty->Struct->GetCppStructOps() && StructProperty->Struct->GetCppStructOps()->HasImportTextItem())
        {
            UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();

            FString ImportTextString = JsonValue->AsString();
            const TCHAR* ImportTextPtr = *ImportTextString;
            if (!TheCppStructOps->ImportTextItem(ImportTextPtr, OutValue, PPF_None, nullptr, reinterpret_cast<FOutputDevice*>(GWarn)))
            {
                // Fall back to trying the tagged property approach if custom ImportTextItem couldn't get it done
                Property->ImportText(ImportTextPtr, OutValue, PPF_None, nullptr);
            }
        }
        else if (JsonValue->Type == EJson::String)
        {
            FString ImportTextString = JsonValue->AsString();
            const TCHAR* ImportTextPtr = *ImportTextString;
            Property->ImportText(ImportTextPtr, OutValue, PPF_None, nullptr);
        }
        else
        {
            UE_LOG(
                LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import UStruct from non-object JSON key for property %s"), *Property->GetNameCPP());
            return false;
        }
    }
    else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
    {
        if (JsonValue->Type == EJson::Object)
        {
            UObject* Outer = GetTransientPackage();
            if (ContainerStruct->IsChildOf(UObject::StaticClass()))
            {
                Outer = static_cast<UObject*>(Container);
            }

            TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();
            UClass* PropertyClass = ObjectProperty->PropertyClass;

            // If a specific subclass was stored in the Json, use that instead of the PropertyClass
            FString ClassString = Obj->GetStringField(ObjectClassNameKey);
            Obj->RemoveField(ObjectClassNameKey);
            if (!ClassString.IsEmpty())
            {
                if (UClass* FoundClass = FindObject<UClass>(ANY_PACKAGE, *ClassString))
                {
                    PropertyClass = FoundClass;
                }
            }

            UObject* CreatedObj = StaticAllocateObject(PropertyClass, Outer, NAME_None, EObjectFlags::RF_NoFlags, EInternalObjectFlags::None, false);
            (*PropertyClass->ClassConstructor)(FObjectInitializer(CreatedObj, PropertyClass->ClassDefaultObject, false, false));

            ObjectProperty->SetObjectPropertyValue(OutValue, CreatedObj);

            check(Obj.IsValid());    // should not fail if Type == EJson::Object
            if (!JsonAttributesToUStructWithContainer(Obj->Values, PropertyClass, CreatedObj, PropertyClass, CreatedObj))
            {
                UE_LOG(
                    LogJson,
                    Error,
                    TEXT("JsonValueToUProperty - JsonObjectDeserialization::JsonObjectToUStruct failed for property %s"),
                    *Property->GetNameCPP());
                return false;
            }
        }
        else if (JsonValue->Type == EJson::String)
        {
            // Default to expect a string for everything else
            if (Property->ImportText(*JsonValue->AsString(), OutValue, 0, nullptr) == nullptr)
            {
                UE_LOG(
                    LogJson,
                    Error,
                    TEXT("JsonValueToUProperty - Unable import property type %s from string value for property %s"),
                    *Property->GetClass()->GetName(),
                    *Property->GetNameCPP());
                return false;
            }
        }
    }
    else
    {
        // Default to expect a string for everything else
        if (Property->ImportText(*JsonValue->AsString(), OutValue, 0, nullptr) == nullptr)
        {
            UE_LOG(
                LogJson,
                Error,
                TEXT("JsonValueToUProperty - Unable import property type %s from string value for property %s"),
                *Property->GetClass()->GetName(),
                *Property->GetNameCPP());
            return false;
        }
    }

    return true;
}

bool JsonValueToFPropertyWithContainer(
    const TSharedPtr<FJsonValue>& JsonValue,
    FProperty* Property,
    void* OutValue,
    const UStruct* ContainerStruct,
    void* Container)
{
    if (!JsonValue.IsValid())
    {
        UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Invalid value JSON key"));
        return false;
    }

    const bool bArrayOrSetProperty = Property->IsA<FArrayProperty>() || Property->IsA<FSetProperty>();
    const bool bJsonArray = JsonValue->Type == EJson::Array;

    if (!bJsonArray)
    {
        if (bArrayOrSetProperty)
        {
            UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import TArray from non-array JSON key"));
            return false;
        }

        if (Property->ArrayDim != 1)
        {
            UE_LOG(LogJson, Warning, TEXT("Ignoring excess properties when deserializing %s"), *Property->GetName());
        }

        return ConvertScalarJsonValueToFPropertyWithContainer(JsonValue, Property, OutValue, ContainerStruct, Container);
    }

    // In practice, the ArrayDim == 1 check ought to be redundant, since nested arrays of FPropertys are not supported
    if (bArrayOrSetProperty && Property->ArrayDim == 1)
    {
        // Read into TArray
        return ConvertScalarJsonValueToFPropertyWithContainer(JsonValue, Property, OutValue, ContainerStruct, Container);
    }

    // We're deserializing a JSON array
    const auto& ArrayValue = JsonValue->AsArray();
    if (Property->ArrayDim < ArrayValue.Num())
    {
        UE_LOG(LogJson, Warning, TEXT("Ignoring excess properties when deserializing %s"), *Property->GetName());
    }

    // Read into native array
    const int ItemsToRead = FMath::Clamp(ArrayValue.Num(), 0, Property->ArrayDim);
    for (int Index = 0; Index != ItemsToRead; ++Index)
    {
        if (!ConvertScalarJsonValueToFPropertyWithContainer(
                ArrayValue[Index], Property, static_cast<char*>(OutValue) + Index * Property->ElementSize, ContainerStruct, Container))
        {
            return false;
        }
    }
    return true;
}

bool JsonAttributesToUStructWithContainer(
    const TMap<FString, TSharedPtr<FJsonValue>>& JsonAttributes,
    const UStruct* StructDefinition,
    void* OutStruct,
    const UStruct* ContainerStruct,
    void* Container)
{
    if (StructDefinition == FJsonObjectWrapper::StaticStruct())
    {
        // Just copy it into the object
        FJsonObjectWrapper* ProxyObject = static_cast<FJsonObjectWrapper*>(OutStruct);
        ProxyObject->JsonObject = MakeShared<FJsonObject>();
        ProxyObject->JsonObject->Values = JsonAttributes;
        return true;
    }

    int32 NumUnclaimedProperties = JsonAttributes.Num();
    if (NumUnclaimedProperties <= 0)
    {
        return true;
    }

    // iterate over the struct properties
    for (TFieldIterator<FProperty> PropIt(StructDefinition); PropIt; ++PropIt)
    {
        FProperty* Property = *PropIt;

        // find a json value matching this property name
        const FString PropertyName = Property->GetName();
        const TSharedPtr<FJsonValue>* JsonValue = JsonAttributes.Find(PropertyName);
        // Try again stripping things like 'b'
        if (!JsonValue && CastField<FBoolProperty>(Property))
        {
            JsonValue = JsonAttributes.Find(NamingConventionConversion::ConvertPropertyNameToUpperCamelCase(PropertyName));
        }
        // Try again converting from snake_case
        if (!JsonValue)
        {
            JsonValue = JsonAttributes.Find(NamingConventionConversion::ConvertPropertyNameToSnakeCase(PropertyName));
        }
        if (!JsonValue)
        {
            // we allow values to not be found since this mirrors the typical UObject mantra that all the fields are
            // optional when deserializing
            continue;
        }

        if (JsonValue->IsValid() && !(*JsonValue)->IsNull())
        {
            void* Value = Property->ContainerPtrToValuePtr<uint8>(OutStruct);
            if (!JsonValueToFPropertyWithContainer(*JsonValue, Property, Value, ContainerStruct, Container))
            {
                UE_LOG(LogJson, Error, TEXT("JsonObjectToUStruct - Unable to parse %s.%s from JSON"), *StructDefinition->GetName(), *Property->GetName());
                return false;
            }
        }

        if (--NumUnclaimedProperties <= 0)
        {
            // If we found all properties that were in the JsonAttributes map, there is no reason to keep looking for
            // more.
            break;
        }
    }

    return true;
}
}    // namespace

bool JsonObjectDeserialization::JsonObjectToUStruct(const TSharedRef<FJsonObject>& JsonObject, const UStruct* StructDefinition, void* OutStruct)
{
    return JsonAttributesToUStruct(JsonObject->Values, StructDefinition, OutStruct);
}

bool JsonObjectDeserialization::JsonAttributesToUStruct(
    const TMap<FString, TSharedPtr<FJsonValue>>& JsonAttributes,
    const UStruct* StructDefinition,
    void* OutStruct)
{
    return JsonAttributesToUStructWithContainer(JsonAttributes, StructDefinition, OutStruct, StructDefinition, OutStruct);
}

bool JsonObjectDeserialization::JsonObjectStringToJsonObject(const FString& JsonString, TSharedPtr<FJsonObject>& OutObject)
{
    const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
    return FJsonSerializer::Deserialize(JsonReader, OutObject) && OutObject.IsValid();
}
