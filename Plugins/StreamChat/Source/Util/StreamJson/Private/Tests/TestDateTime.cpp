// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "StreamJson.h"
#include "TestJson.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJsonDateTimeTest,
    "StreamChat.StreamJson.Deserialize.DateTime",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FJsonDateTimeTest::RunTest(const FString& Parameters)
{
    {
        const FString DateString{TEXT("2022-02-02T09:41:36.999964Z")};
        FDateTime DateTime;
        TestTrue("Parsed", JsonObjectDeserialization::ParseIso8601(*DateString, DateTime));
        TestEqual("Year", DateTime.GetYear(), 2022);
        TestEqual("Month", DateTime.GetMonth(), 2);
        TestEqual("Day", DateTime.GetDay(), 2);
        TestEqual("Hour", DateTime.GetHour(), 9);
        TestEqual("Minute", DateTime.GetMinute(), 41);
        TestEqual("Second", DateTime.GetSecond(), 37);
        TestEqual("Millisecond", DateTime.GetMillisecond(), 0);
    }
    {
        const FString DateString{TEXT("2022-12-31T23:59:59.999Z")};
        FDateTime DateTime;
        TestTrue("Parsed", JsonObjectDeserialization::ParseIso8601(*DateString, DateTime));
        TestEqual("Year", DateTime.GetYear(), 2022);
        TestEqual("Month", DateTime.GetMonth(), 12);
        TestEqual("Day", DateTime.GetDay(), 31);
        TestEqual("Hour", DateTime.GetHour(), 23);
        TestEqual("Minute", DateTime.GetMinute(), 59);
        TestEqual("Second", DateTime.GetSecond(), 59);
        TestEqual("Millisecond", DateTime.GetMillisecond(), 999);
    }
    {
        const FString DateString{TEXT("2022-12-31T23:59:59.9999Z")};
        FDateTime DateTime;
        TestTrue("Parsed", JsonObjectDeserialization::ParseIso8601(*DateString, DateTime));
        TestEqual("Year", DateTime.GetYear(), 2023);
        TestEqual("Month", DateTime.GetMonth(), 1);
        TestEqual("Day", DateTime.GetDay(), 1);
        TestEqual("Hour", DateTime.GetHour(), 0);
        TestEqual("Minute", DateTime.GetMinute(), 0);
        TestEqual("Second", DateTime.GetSecond(), 0);
        TestEqual("Millisecond", DateTime.GetMillisecond(), 0);
    }
    return true;
}

#endif
