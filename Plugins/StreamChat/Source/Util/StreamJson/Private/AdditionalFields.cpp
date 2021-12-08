// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "AdditionalFields.h"

void FAdditionalFields::SetString(const FString& Key, const FString& Value)
{
    Inner.Add(Key, MakeShared<FJsonValueString>(Value));
}

void FAdditionalFields::SetString(const FString& Key, const TCHAR* Value)
{
    Inner.Add(Key, MakeShared<FJsonValueString>(Value));
}

void FAdditionalFields::SetBool(const FString& Key, bool bValue)
{
    Inner.Add(Key, MakeShared<FJsonValueBoolean>(bValue));
}

void FAdditionalFields::SetJsonObject(const FString& Key, const TSharedRef<FJsonValue>& Value)
{
    Inner.Add(Key, Value);
}

template <>
void FAdditionalFields::Set(const FString& Key, const FDateTime& Value)
{
    Inner.Add(Key, MakeShared<FJsonValueString>(Value.ToIso8601()));
}

TOptional<FString> FAdditionalFields::GetString(const FString& Key) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Key))
    {
        if (FString OutString; JsonValue->Get()->TryGetString(OutString))
        {
            return {OutString};
        }
    }
    return {};
}

TOptional<bool> FAdditionalFields::GetBool(const FString& Key) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Key))
    {
        if (bool OutBool; JsonValue->Get()->TryGetBool(OutBool))
        {
            return {OutBool};
        }
    }
    return {};
}

template <>
TOptional<FDateTime> FAdditionalFields::Get(const FString& Key) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Key))
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

const TMap<FString, TSharedPtr<FJsonValue>>& FAdditionalFields::GetFields() const
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

void UAdditionalFieldsBlueprintLibrary::SetString(FAdditionalFields& AdditionalFields, const FString& Key, const FString& Value, FAdditionalFields& Out)
{
    AdditionalFields.SetString(Key, Value);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::SetFloat(FAdditionalFields& AdditionalFields, const FString& Key, const float Value, FAdditionalFields& Out)
{
    AdditionalFields.SetNumber(Key, Value);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::SetInt(FAdditionalFields& AdditionalFields, const FString& Key, const int32 Value, FAdditionalFields& Out)
{
    AdditionalFields.SetNumber(Key, Value);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::SetBool(FAdditionalFields& AdditionalFields, const FString& Key, bool bValue, FAdditionalFields& Out)
{
    AdditionalFields.SetBool(Key, bValue);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::GetString(const FAdditionalFields& AdditionalFields, const FString& Key, bool& bValid, FString& Result)
{
    const TOptional<FString> Optional = AdditionalFields.GetString(Key);
    bValid = Optional.IsSet();
    if (!bValid)
    {
        return;
    }
    Result = Optional.GetValue();
}

void UAdditionalFieldsBlueprintLibrary::GetFloat(const FAdditionalFields& AdditionalFields, const FString& Key, bool& bValid, float& Result)
{
    const TOptional<float> Optional = AdditionalFields.GetNumber<float>(Key);
    bValid = Optional.IsSet();
    if (!bValid)
    {
        return;
    }
    Result = Optional.GetValue();
}

void UAdditionalFieldsBlueprintLibrary::GetInt(const FAdditionalFields& AdditionalFields, const FString& Key, bool& bValid, int32& Result)
{
    const TOptional<int32> Optional = AdditionalFields.GetNumber<int32>(Key);
    bValid = Optional.IsSet();
    if (!bValid)
    {
        return;
    }
    Result = Optional.GetValue();
}

void UAdditionalFieldsBlueprintLibrary::GetBool(const FAdditionalFields& AdditionalFields, const FString& Key, bool& bValid, bool& Result)
{
    const TOptional<bool> Optional = AdditionalFields.GetBool(Key);
    bValid = Optional.IsSet();
    if (!bValid)
    {
        return;
    }
    Result = Optional.GetValue();
}
