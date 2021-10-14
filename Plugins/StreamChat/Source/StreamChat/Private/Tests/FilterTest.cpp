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
        const UFilter* Filter = UFilter::Equal(TEXT("key"), FString(TEXT("value")));
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$eq":"value"}})")};
        TestEqual("$eq", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::Greater(TEXT("key"), 4);
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$gt":4}})")};
        TestEqual("$gt", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::GreaterOrEqual(TEXT("key"), FString(TEXT("value")));
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$gte":"value"}})")};
        TestEqual("$gte", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::Less(TEXT("key"), 4);
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$lt":4}})")};
        TestEqual("$lt", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::LessOrEqual(TEXT("key"), FString(TEXT("value")));
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$lte":"value"}})")};
        TestEqual("$lte", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::NotEqual(TEXT("key"), 4);
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$ne":4}})")};
        TestEqual("$ne", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::In(TEXT("key"), TArray{1, 2, 4});
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$in":[1,2,4]}})")};
        TestEqual("$in", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::NotIn(TEXT("key"), TArray{3, 5, 7});
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"key":{"$nin":[3,5,7]}})")};
        TestEqual("$nin", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::And({
            UFilter::In(TEXT("key"), TArray{1, 2, 4}),
            UFilter::Equal(TEXT("some_other_key"), 10),
        });
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"$and":[{"key":{"$in":[1,2,4]}},{"some_other_key":{"$eq":10}}]})")};
        TestEqual("$ne", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::Or({
            UFilter::In(TEXT("key"), TArray{1, 2, 4}),
            UFilter::Equal(TEXT("key2"), 10),
        });
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"$or":[{"key":{"$in":[1,2,4]}},{"key2":{"$eq":10}}]})")};
        TestEqual("$ne", Json, ExpectedJson);
    }
    {
        const UFilter* Filter = UFilter::Nor({
            UFilter::In(TEXT("key"), TArray{1, 2, 4}),
            UFilter::Equal(TEXT("key2"), 10),
        });
        const FString Json = Filter->ToJson();
        const FString ExpectedJson{TEXT(R"({"$nor":[{"key":{"$in":[1,2,4]}},{"key2":{"$eq":10}}]})")};
        TestEqual("$ne", Json, ExpectedJson);
    }

    return true;
}
#endif
