// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AdditionalFields.generated.h"

USTRUCT(BlueprintType)
struct FAdditionalFields
{
    GENERATED_BODY()

public:
    void AddString(const FString& Key, const FString& Value);
    void AddNumber(const FString& Key, float Value);
    void AddBool(const FString& Key, bool bValue);

    const TMap<FString, TSharedPtr<FJsonValue>>& GetFields() const;

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
