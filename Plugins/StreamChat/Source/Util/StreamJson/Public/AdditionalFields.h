// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Detail/JsonObjectDeserialization.h"
#include "Dom/JsonValue.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StreamJson.h"

#include "AdditionalFields.generated.h"

USTRUCT(BlueprintType)
struct STREAMJSON_API FAdditionalFields
{
    GENERATED_BODY()

public:
    void SetString(const FString& Key, const FString& Value);
    void SetString(const FString& Key, const TCHAR* Value);
    void SetBool(const FString& Key, bool bValue);
    void SetJsonObject(const FString& Key, const TSharedRef<FJsonValue>&);
    template <class T>
    void SetNumber(const FString& Key, T Value);
    template <class T>
    void Set(const FString& Key, const T& Value);

    TOptional<FString> GetString(const FString& Key) const;
    TOptional<bool> GetBool(const FString& Key) const;
    template <class T>
    TOptional<T> GetNumber(const FString& Key) const;
    template <class T>
    TOptional<T> Get(const FString& Key) const;

    const TMap<FString, TSharedPtr<FJsonValue>>& GetFields() const;

    static FAdditionalFields* FromProperty(void* Struct, FProperty* Property);
    static const FAdditionalFields* FromProperty(const void* Struct, FProperty* Property);

private:
    TMap<FString, TSharedPtr<FJsonValue>> Inner;
};

/**
 * @brief Blueprint functions for the Additional Fields struct
 * @ingroup StreamChat
 */
UCLASS()
class STREAMJSON_API UAdditionalFieldsBlueprintLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void AddString(UPARAM(ref) FAdditionalFields& AdditionalFields, const FString& Key, const FString& Value, FAdditionalFields& Out);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void AddNumber(UPARAM(ref) FAdditionalFields& AdditionalFields, const FString& Key, float Value, FAdditionalFields& Out);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void AddBool(UPARAM(ref) FAdditionalFields& AdditionalFields, const FString& Key, bool bValue, FAdditionalFields& Out);
};

// General implementation for all number types
template <class T>
void FAdditionalFields::SetNumber(const FString& Key, T Value)
{
    Inner.Add(Key, MakeShared<FJsonValueNumber>(Value));
}

template <class T>
void FAdditionalFields::Set(const FString& Key, const T& Value)
{
    const TSharedRef<FJsonObject> Object = JsonObject::UStructToJsonObject<T>(Value);
    Inner.Add(Key, MakeShared<FJsonValueObject>(Object));
}

template <class T>
TOptional<T> FAdditionalFields::GetNumber(const FString& Key) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Key))
    {
        if (T OutNumber; JsonValue->Get()->TryGetNumber(OutNumber))
        {
            return {OutNumber};
        }
    }
    return {};
}

template <class T>
TOptional<T> FAdditionalFields::Get(const FString& Key) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Key))
    {
        if (const TSharedPtr<FJsonObject>* OutObject; JsonValue->Get()->TryGetObject(OutObject))
        {
            T OutStruct;
            if (JsonObjectDeserialization::JsonObjectToUStruct(OutObject->ToSharedRef(), &OutStruct))
            {
                return {OutStruct};
            }
        }
    }
    return {};
}

template <>
TOptional<FDateTime> STREAMJSON_API FAdditionalFields::Get(const FString& Key) const;
template <>
void STREAMJSON_API FAdditionalFields::Set(const FString& Key, const FDateTime&);
