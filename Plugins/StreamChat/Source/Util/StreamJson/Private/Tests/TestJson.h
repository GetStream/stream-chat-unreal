// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"

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
    ETestEnum ManyWordsEnum;
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
    float Bar;
};

USTRUCT()
struct FTestJson
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Int32;

    UPROPERTY()
    int32 Int64;

    UPROPERTY()
    float Float;

    UPROPERTY()
    double Double;

    UPROPERTY()
    bool bBoolean;

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
    ETestEnum Enum;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
USTRUCT()
struct FSmallTestJson
{
    GENERATED_BODY()

    UPROPERTY()
    float Number;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
