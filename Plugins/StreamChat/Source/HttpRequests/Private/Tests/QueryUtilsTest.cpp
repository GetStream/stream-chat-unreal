#include "Containers/UnrealString.h"
#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "QueryUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FParseQueryTest,
    "StreamChat.HttpRequests.QueryUtils.Parse Query",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FParseQueryTest::RunTest(const FString& Parameters)
{
    {
        const FString Url = TEXT("google.com/query?a=1&b=2&c=3");
        const TMap<FString, FString> Params = QueryUtils::ParseQuery(Url);

        TestEqual("a", Params[TEXT("a")], TEXT("1"));
        TestEqual("b", Params[TEXT("b")], TEXT("2"));
        TestEqual("c", Params[TEXT("c")], TEXT("3"));
    }
    {
        const FString Url = TEXT("https://www.yahoo.com/site/great?foo%20e=a%20a&foo a=3#end");
        const TMap<FString, FString> Params = QueryUtils::ParseQuery(Url);

        TestEqual("Map size", Params.Num(), 2);
        TestEqual("foo%20e", Params[TEXT("foo e")], TEXT("a a"));
        TestEqual("food a", Params[TEXT("foo a")], TEXT("3"));
    }

    return true;
}

#endif
