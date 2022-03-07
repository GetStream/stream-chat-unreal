// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Filter.h"
#include "CoreTypes.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FConvertFilterToJsonTest,
    "StreamChat.Filter.Convert to JSON",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FConvertFilterToJsonTest::RunTest(const FString& Parameters)
{
    {
        const FFilter Filter = FFilter::Equal(TEXT("key"), FString(TEXT("value")));
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$eq":"value"}})")};
        TestEqual("$eq", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::Greater(TEXT("key"), 4);
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$gt":4}})")};
        TestEqual("$gt", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::GreaterOrEqual(TEXT("key"), FString(TEXT("value")));
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$gte":"value"}})")};
        TestEqual("$gte", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::Less(TEXT("key"), 4);
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$lt":4}})")};
        TestEqual("$lt", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::LessOrEqual(TEXT("key"), FString(TEXT("value")));
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$lte":"value"}})")};
        TestEqual("$lte", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::NotEqual(TEXT("key"), 4);
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$ne":4}})")};
        TestEqual("$ne", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::In(TEXT("key"), TArray<int32>{1, 2, 4});
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$in":[1,2,4]}})")};
        TestEqual("$in", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::NotIn(TEXT("key"), TArray<int32>{3, 5, 7});
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$nin":[3,5,7]}})")};
        TestEqual("$nin", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::And({
            FFilter::In(TEXT("key"), TArray<int32>{1, 2, 4}),
            FFilter::Equal(TEXT("some_other_key"), 10),
        });
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"$and":[{"key":{"$in":[1,2,4]}},{"some_other_key":{"$eq":10}}]})")};
        TestEqual("$ne", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::Or({
            FFilter::In(TEXT("key"), TArray<int32>{1, 2, 4}),
            FFilter::Equal(TEXT("key2"), 10),
        });
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"$or":[{"key":{"$in":[1,2,4]}},{"key2":{"$eq":10}}]})")};
        TestEqual("$ne", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::Nor({
            FFilter::In(TEXT("key"), TArray<int32>{1, 2, 4}),
            FFilter::Equal(TEXT("key2"), 10),
        });
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"$nor":[{"key":{"$in":[1,2,4]}},{"key2":{"$eq":10}}]})")};
        TestEqual("$ne", Json, ExpectedJson);
    }
    {
        const FFilter Filter = FFilter::Empty(TEXT("team"));
        const FString Json = Filter.ToJson();
        const FString ExpectedJson{TEXT(R"({"team":{}})")};
        TestEqual("empty", Json, ExpectedJson);
    }

    return true;
}
#endif
