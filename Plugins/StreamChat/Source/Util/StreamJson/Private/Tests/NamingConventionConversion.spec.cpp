// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "NamingConventionConversion.h"

#include "Misc/AutomationTest.h"
#include "TestJson.h"

BEGIN_DEFINE_SPEC(
    FNamingConventionConversionSpec,
    "StreamChat.NamingConventionConversion",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
END_DEFINE_SPEC(FNamingConventionConversionSpec)

void FNamingConventionConversionSpec::Define()
{
    It("should convert snake_case to UpperCamelCase",
       EAsyncExecution::ThreadPool,
       [this]()
       {
           const FString SnakeCase = TEXT("wow_such_snake_case");
           const FString Expected = TEXT("WowSuchSnakeCase");
           const FString UpperCamelCase = NamingConventionConversion::ConvertSnakeCaseToUpperCamelCase(SnakeCase);
           TestEqual("camel_case->UpperCamelCase", UpperCamelCase, Expected);
       });

    It("should convert mixed-up snake_case to UpperCamelCase",
       EAsyncExecution::ThreadPool,
       [this]()
       {
           const FString SnakeCase = TEXT("mixed__up_snakey___case_");
           const FString Expected = TEXT("MixedUpSnakeyCase");
           const FString UpperCamelCase = NamingConventionConversion::ConvertSnakeCaseToUpperCamelCase(SnakeCase);
           TestEqual("camel_case->UpperCamelCase", UpperCamelCase, Expected);
       });
}
