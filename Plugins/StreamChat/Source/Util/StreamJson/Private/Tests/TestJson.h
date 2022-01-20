// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "StreamJsonField.h"

#include "TestJson.generated.h"

UENUM()
enum class ETestEnum : uint8
{
    FirstElement,
    SecondEnumValue,
    Number3
};

USTRUCT()
struct FTestEnum
{
    GENERATED_BODY()

    UPROPERTY()
    ETestEnum ManyWordsEnum = ETestEnum::Number3;
};

USTRUCT()
struct FTestNestedEnum
{
    GENERATED_BODY()

    UPROPERTY()
    FTestEnum NestedEnum;
};

USTRUCT()
struct FTestNestedJson
{
    GENERATED_BODY()

    UPROPERTY()
    FString Foo;

    UPROPERTY()
    float Bar = -1.f;
};

USTRUCT()
struct FTestJson
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Int32 = -1;

    UPROPERTY()
    int32 Int64 = -1;

    UPROPERTY()
    float Float = -1.f;

    UPROPERTY()
    double Double = -1.;

    UPROPERTY()
    bool bBoolean = false;

    UPROPERTY()
    FString String;

    UPROPERTY()
    FTestNestedJson NestedObject;

    UPROPERTY()
    TArray<int32> ArrayOfInt32;

    UPROPERTY()
    TArray<int32> ArrayOfInt64;

    UPROPERTY()
    TArray<float> ArrayOfFloat;

    UPROPERTY()
    TArray<double> ArrayOfDouble;

    UPROPERTY()
    TArray<bool> ArrayOfBoolean;

    UPROPERTY()
    TArray<FString> ArrayOfString;

    UPROPERTY()
    TArray<FTestNestedJson> ArrayOfNestedObject;

    UPROPERTY()
    ETestEnum Enum = ETestEnum::Number3;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};

USTRUCT()
struct FSmallTestJson
{
    GENERATED_BODY()

    UPROPERTY()
    float Number = -1.f;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
USTRUCT()
struct FExtraFieldTestJson
{
    GENERATED_BODY()

    TOptional<FDateTime> GetDeletedAt() const
    {
        return AdditionalFields.Get<FDateTime>(TEXT("deleted_at"));
    }
    void SetDeletedAt(const FDateTime& Value)
    {
        return AdditionalFields.Set(TEXT("deleted_at"), Value);
    }

    UPROPERTY()
    FString String;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
