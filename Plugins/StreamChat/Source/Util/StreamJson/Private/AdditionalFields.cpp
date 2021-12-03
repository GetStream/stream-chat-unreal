#include "AdditionalFields.h"

void FAdditionalFields::AddString(const FString& Key, const FString& Value)
{
    Inner.Add(Key, MakeShared<FJsonValueString>(Value));
}

void FAdditionalFields::AddNumber(const FString& Key, float Value)
{
    Inner.Add(Key, MakeShared<FJsonValueNumber>(Value));
}

void FAdditionalFields::AddBool(const FString& Key, bool bValue)
{
    Inner.Add(Key, MakeShared<FJsonValueBoolean>(bValue));
}

const TMap<FString, TSharedPtr<FJsonValue>>& FAdditionalFields::GetFields() const
{
    return Inner;
}

void UAdditionalFieldsBlueprintLibrary::AddString(FAdditionalFields& AdditionalFields, const FString& Key, const FString& Value, FAdditionalFields& Out)
{
    AdditionalFields.AddString(Key, Value);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::AddNumber(FAdditionalFields& AdditionalFields, const FString& Key, float Value, FAdditionalFields& Out)
{
    AdditionalFields.AddNumber(Key, Value);
    Out = AdditionalFields;
}

void UAdditionalFieldsBlueprintLibrary::AddBool(FAdditionalFields& AdditionalFields, const FString& Key, bool bValue, FAdditionalFields& Out)
{
    AdditionalFields.AddNumber(Key, bValue);
    Out = AdditionalFields;
}
