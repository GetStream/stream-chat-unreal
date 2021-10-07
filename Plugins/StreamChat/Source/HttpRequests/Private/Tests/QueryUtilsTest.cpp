#include "Containers/UnrealString.h"
#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "QueryUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FParseQueryTest,
    "StreamChat.HttpRequests.QueryUtils.Parse query",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FParseQueryTest::RunTest(const FString& Parameters)
{
    {
        const FString Url = TEXT("google.com/query?a=1&b=2&c=3");
        const TMap<FString, FString> Params = QueryUtils::ParseQueryFromUrl(Url);

        TestEqual("a", Params[TEXT("a")], TEXT("1"));
        TestEqual("b", Params[TEXT("b")], TEXT("2"));
        TestEqual("c", Params[TEXT("c")], TEXT("3"));
    }
    {
        const FString Url = TEXT("https://www.yahoo.com/site/great?foo%20e=a%20a&foo a=3#end");
        const TMap<FString, FString> Params = QueryUtils::ParseQueryFromUrl(Url);

        TestEqual("Map size", Params.Num(), 2);
        TestEqual("foo%20e", Params[TEXT("foo e")], TEXT("a a"));
        TestEqual("food a", Params[TEXT("foo a")], TEXT("3"));
    }

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAddQueryToUrlTest,
    "StreamChat.HttpRequests.QueryUtils.Add query to URL",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FAddQueryToUrlTest::RunTest(const FString& Parameters)
{
    {
        const FString Url = TEXT("google.com/query?a=1&b=2&c=3#frag");
        const FString NewUrl = QueryUtils::AddQueryToUrl(
            Url,
            {
                {TEXT("a"), TEXT("new_a")},
                {TEXT("c"), 0.5f},
                {TEXT("d"), 9},
            });

        TestEqual("New URL", NewUrl, TEXT("google.com/query?a=new_a&b=2&c=0.5&d=9#frag"));
    }

    return true;
}

#endif
