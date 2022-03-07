// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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
    void SetString(const FName& Field, const FString& Value);
    void SetString(const FName& Field, const TCHAR* Value);
    void SetBool(const FName& Field, bool bValue);
    void SetJsonValue(const FName& Field, const TSharedRef<FJsonValue>&);
    template <class T>
    void SetNumber(const FName& Field, T Value);
    template <class T>
    void SetArray(const FName& Field, const TArray<T>& Value);
    /// Set JSON object field using a USTRUCT
    template <class T>
    void Set(const FName& Field, const T& Value);

    TOptional<FString> GetString(const FName& Field) const;
    TOptional<bool> GetBool(const FName& Field) const;
    template <class T>
    TOptional<T> GetNumber(const FName& Field) const;
    /// Get JSON object field using a USTRUCT
    template <class T>
    TOptional<T> Get(const FName& Field) const;

    const TMap<FName, TSharedPtr<FJsonValue>>& GetFields() const;

    static FAdditionalFields* FromProperty(void* Struct, FProperty* Property);
    static const FAdditionalFields* FromProperty(const void* Struct, FProperty* Property);

private:
    TMap<FName, TSharedPtr<FJsonValue>> Inner;
};

// General implementation for all number types
template <class T>
void FAdditionalFields::SetNumber(const FName& Field, T Value)
{
    Inner.Add(Field, MakeShared<FJsonValueNumber>(Value));
}

template <class T>
void FAdditionalFields::Set(const FName& Field, const T& Value)
{
    const TSharedRef<FJsonObject> Object = JsonObject::UStructToJsonObject<T>(Value);
    Inner.Add(Field, MakeShared<FJsonValueObject>(Object));
}

template <class T>
void FAdditionalFields::SetArray(const FName& Field, const TArray<T>& Value)
{
    TArray<TSharedPtr<FJsonValue>> JsonValues;
    JsonValues.Reserve(Value.Num());
    for (const T& Elem : Value)
    {
        const TSharedRef<FJsonObject> JsonObject = JsonObject::UStructToJsonObject<T>(Elem);
        JsonValues.Add(MakeShared<FJsonValueObject>(JsonObject));
    }
    Inner.Add(Field, MakeShared<FJsonValueArray>(JsonValues));
}

template <class T>
TOptional<T> FAdditionalFields::GetNumber(const FName& Field) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Field))
    {
        if (T OutNumber; JsonValue->Get()->TryGetNumber(OutNumber))
        {
            return {OutNumber};
        }
    }
    return {};
}

template <class T>
TOptional<T> FAdditionalFields::Get(const FName& Field) const
{
    if (const TSharedPtr<FJsonValue>* JsonValue = Inner.Find(Field))
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
TOptional<FDateTime> STREAMJSON_API FAdditionalFields::Get(const FName& Field) const;
template <>
void STREAMJSON_API FAdditionalFields::Set(const FName& Field, const FDateTime&);

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
    static void SetString(UPARAM(ref) FAdditionalFields& AdditionalFields, FName Field, const FString& Value, FAdditionalFields& Out);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void SetFloat(UPARAM(ref) FAdditionalFields& AdditionalFields, FName Field, float Value, FAdditionalFields& Out);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void SetInt(UPARAM(ref) FAdditionalFields& AdditionalFields, FName Field, int32 Value, FAdditionalFields& Out);
    UFUNCTION(BlueprintPure, Category = "Stream Chat|Additional Fields")
    static void SetBool(UPARAM(ref) FAdditionalFields& AdditionalFields, FName Field, bool bValue, FAdditionalFields& Out);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Additional Fields", meta = (ReturnDisplayName = Success))
    static bool GetString(const FAdditionalFields& AdditionalFields, FName Field, FString& Result);
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Additional Fields", meta = (ReturnDisplayName = Success))
    static bool GetFloat(const FAdditionalFields& AdditionalFields, FName Field, float& Result);
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Additional Fields", meta = (ReturnDisplayName = Success))
    static bool GetInt(const FAdditionalFields& AdditionalFields, FName Field, int32& Result);
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Additional Fields", meta = (ReturnDisplayName = Success))
    static bool GetBool(const FAdditionalFields& AdditionalFields, FName Field, bool& Result);
};
