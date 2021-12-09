// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "AdditionalFields.h"

void FAdditionalFields::SetString(const FName& Field, const FString& Value)
{
    Inner.Add(Field, MakeShared<FJsonValueString>(Value));
}

void FAdditionalFields::SetString(const FName& Field, const TCHAR* Value)
{
    Inner.Add(Field, MakeShared<FJsonValueString>(Value));
}

void FAdditionalFields::SetBool(const FName& Field, bool bValue)
{
    Inner.Add(Field, MakeShared<FJsonValueBoolean>(bValue));
}

void FAdditionalFields::SetJsonValue(const FName& Field, const TSharedRef<FJsonValue>& Value)
{
    Inner.Add(Field, Value);
}

template <>
void FAdditionalFields::Set(const FName& Field, const FDateTime& Value)
{
    Inner.Add(Field, MakeShared<FJsonValueString>(Value.ToIso8601()));
}

TOptional<FString> FAdditionalFields::GetString(const FName& Field) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Field))
    {
        if (FString OutString; JsonValue->Get()->TryGetString(OutString))
        {
            return {OutString};
        }
    }
    return {};
}

TOptional<bool> FAdditionalFields::GetBool(const FName& Field) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Field))
    {
        if (bool OutBool; JsonValue->Get()->TryGetBool(OutBool))
        {
            return {OutBool};
        }
    }
    return {};
}

template <>
TOptional<FDateTime> FAdditionalFields::Get(const FName& Field) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Field))
    {
        if (FString OutString; JsonValue->Get()->TryGetString(OutString))
        {
            if (FDateTime Date; FDateTime::ParseIso8601(*OutString, Date))
            {
                return {Date};
            }
        }
    }
    return {};
}

const TMap<FName, TSharedPtr<FJsonValue>>& FAdditionalFields::GetFields() const
{
    return Inner;
}

FAdditionalFields* FAdditionalFields::FromProperty(void* Struct, FProperty* Property)
{
    static const FName AdditionalFieldsName = TEXT("AdditionalFields");
    if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
    {
        if (StructProperty->Struct->GetFName() == AdditionalFieldsName)
        {
            return StructProperty->ContainerPtrToValuePtr<FAdditionalFields>(Struct);
        }
    }
    return nullptr;
}

const FAdditionalFields* FAdditionalFields::FromProperty(const void* Struct, FProperty* Property)
{
    return FromProperty(const_cast<void*>(Struct), Property);
}

void UAdditionalFieldsBlueprintLibrary::SetString(FAdditionalFields& AdditionalFields, const FName Field, const FString& Value, FAdditionalFields& Out)
{
    AdditionalFields.SetString(Field, Value);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::SetFloat(FAdditionalFields& AdditionalFields, const FName Field, const float Value, FAdditionalFields& Out)
{
    AdditionalFields.SetNumber(Field, Value);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::SetInt(FAdditionalFields& AdditionalFields, const FName Field, const int32 Value, FAdditionalFields& Out)
{
    AdditionalFields.SetNumber(Field, Value);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::SetBool(FAdditionalFields& AdditionalFields, const FName Field, const bool bValue, FAdditionalFields& Out)
{
    AdditionalFields.SetBool(Field, bValue);
    Out = AdditionalFields;
}

bool UAdditionalFieldsBlueprintLibrary::GetString(const FAdditionalFields& AdditionalFields, const FName Field, FString& Result)
{
    const TOptional<FString> Optional = AdditionalFields.GetString(Field);
    if (!Optional.IsSet())
    {
        return false;
    }
    Result = Optional.GetValue();
    return true;
}

bool UAdditionalFieldsBlueprintLibrary::GetFloat(const FAdditionalFields& AdditionalFields, const FName Field, float& Result)
{
    const TOptional<float> Optional = AdditionalFields.GetNumber<float>(Field);
    if (!Optional.IsSet())
    {
        return false;
    }
    Result = Optional.GetValue();
    return true;
}

bool UAdditionalFieldsBlueprintLibrary::GetInt(const FAdditionalFields& AdditionalFields, const FName Field, int32& Result)
{
    const TOptional<int32> Optional = AdditionalFields.GetNumber<int32>(Field);
    if (!Optional.IsSet())
    {
        return false;
    }
    Result = Optional.GetValue();
    return true;
}

bool UAdditionalFieldsBlueprintLibrary::GetBool(const FAdditionalFields& AdditionalFields, const FName Field, bool& Result)
{
    const TOptional<bool> Optional = AdditionalFields.GetBool(Field);
    if (!Optional.IsSet())
    {
        return false;
    }
    Result = Optional.GetValue();
    return true;
}
