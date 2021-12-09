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
    void SetString(const FName& Key, const FString& Value);
    void SetString(const FName& Key, const TCHAR* Value);
    void SetBool(const FName& Key, bool bValue);
    void SetJsonValue(const FName& Key, const TSharedRef<FJsonValue>&);
    template <class T>
    void SetNumber(const FName& Key, T Value);
    template <class T>
    void SetArray(const FName& Key, const TArray<T>& Value);
    template <class T>
    void Set(const FName& Key, const T& Value);

    TOptional<FString> GetString(const FName& Key) const;
    TOptional<bool> GetBool(const FName& Key) const;
    template <class T>
    TOptional<T> GetNumber(const FName& Key) const;
    template <class T>
    TOptional<T> Get(const FName& Key) const;

    const TMap<FName, TSharedPtr<FJsonValue>>& GetFields() const;

    static FAdditionalFields* FromProperty(void* Struct, FProperty* Property);
    static const FAdditionalFields* FromProperty(const void* Struct, FProperty* Property);

private:
    TMap<FName, TSharedPtr<FJsonValue>> Inner;
};

// General implementation for all number types
template <class T>
void FAdditionalFields::SetNumber(const FName& Key, T Value)
{
    Inner.Add(Key, MakeShared<FJsonValueNumber>(Value));
}

template <class T>
void FAdditionalFields::Set(const FName& Key, const T& Value)
{
    const TSharedRef<FJsonObject> Object = JsonObject::UStructToJsonObject<T>(Value);
    Inner.Add(Key, MakeShared<FJsonValueObject>(Object));
}

template <class T>
void FAdditionalFields::SetArray(const FName& Key, const TArray<T>& Value)
{
    TArray<TSharedPtr<FJsonValue>> JsonValues;
    JsonValues.Reserve(Value.Num());
    for (const T& Elem : Value)
    {
        const TSharedRef<FJsonObject> JsonObject = JsonObject::UStructToJsonObject<T>(Elem);
        JsonValues.Add(MakeShared<FJsonValueObject>(JsonObject));
    }
    Inner.Add(Key, MakeShared<FJsonValueArray>(JsonValues));
}

template <class T>
TOptional<T> FAdditionalFields::GetNumber(const FName& Key) const
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
TOptional<T> FAdditionalFields::Get(const FName& Key) const
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
TOptional<FDateTime> STREAMJSON_API FAdditionalFields::Get(const FName& Key) const;
template <>
void STREAMJSON_API FAdditionalFields::Set(const FName& Key, const FDateTime&);

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
    static void SetString(UPARAM(ref) FAdditionalFields& AdditionalFields, const FName& Key, const FString& Value, FAdditionalFields& Out);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void SetFloat(UPARAM(ref) FAdditionalFields& AdditionalFields, const FName& Key, float Value, FAdditionalFields& Out);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void SetInt(UPARAM(ref) FAdditionalFields& AdditionalFields, const FName& Key, int32 Value, FAdditionalFields& Out);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void SetBool(UPARAM(ref) FAdditionalFields& AdditionalFields, const FName& Key, bool bValue, FAdditionalFields& Out);

    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void GetString(const FAdditionalFields& AdditionalFields, const FName& Key, bool& bValid, FString& Result);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void GetFloat(const FAdditionalFields& AdditionalFields, const FName& Key, bool& bValid, float& Result);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void GetInt(const FAdditionalFields& AdditionalFields, const FName& Key, bool& bValid, int32& Result);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void GetBool(const FAdditionalFields& AdditionalFields, const FName& Key, bool& bValid, bool& Result);
};
