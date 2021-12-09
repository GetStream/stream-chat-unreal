// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelSortOption.h"
#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "Request/SortParamRequestDto.h"
#include "StreamJson.h"
#include "Util.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FSortOptionToJsonTest,
    "StreamChat.SortOption.Convert to JSON",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FSortOptionToJsonTest::RunTest(const FString& Parameters)
{
    {
        const FChannelSortOption SortOption{EChannelSortField::CreatedAt, ESortDirection::Ascending};
        const FSortParamRequestDto SortOptionDto = Util::Convert<FSortParamRequestDto>(SortOption);
        const FString Json = Json::Serialize(SortOptionDto);
        const FString ExpectedJson{TEXT(R"({"field":"created_at","direction":1})")};
        TestEqual("Created At", Json, ExpectedJson);
    }
    {
        const FChannelSortOption SortOption{EChannelSortField::UnreadCount, ESortDirection::Descending};
        const FSortParamRequestDto SortOptionDto = Util::Convert<FSortParamRequestDto>(SortOption);
        const FString Json = Json::Serialize(SortOptionDto);
        const FString ExpectedJson{TEXT(R"({"field":"unread_count","direction":-1})")};
        TestEqual("Unread Count", Json, ExpectedJson);
    }
    return true;
}
#endif
