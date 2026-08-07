// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "NamingConventionConversion.h"

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(
    FNamingConventionConversionSpec,
    "StreamChat.NamingConventionConversion",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
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

    It("should produce the wire keys the attachment and upload endpoints expect",
       EAsyncExecution::ThreadPool,
       [this]()
       {
           // These are the field names on FAttachmentDto and FFileUploadResponseDto. If any of them
           // stops converting to the key Stream uses, uploads silently lose data rather than fail,
           // so they are pinned here.
           const TMap<FString, FString> Expected{
               {TEXT("AssetUrl"), TEXT("asset_url")},
               {TEXT("ImageUrl"), TEXT("image_url")},
               {TEXT("ThumbUrl"), TEXT("thumb_url")},
               {TEXT("MimeType"), TEXT("mime_type")},
               {TEXT("FileSize"), TEXT("file_size")},
               {TEXT("OgScrapeUrl"), TEXT("og_scrape_url")},
               {TEXT("TitleLink"), TEXT("title_link")},
               {TEXT("AuthorName"), TEXT("author_name")},
               {TEXT("AuthorLink"), TEXT("author_link")},
               {TEXT("Attachments"), TEXT("attachments")},
               {TEXT("File"), TEXT("file")},
               {TEXT("Type"), TEXT("type")},
           };
           for (const auto& Pair : Expected)
           {
               TestEqual(*Pair.Key, NamingConventionConversion::ConvertPropertyNameToSnakeCase(Pair.Key), Pair.Value);
           }
       });
}

#endif
