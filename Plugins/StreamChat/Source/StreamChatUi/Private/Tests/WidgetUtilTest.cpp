#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "WidgetUtil.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FHashStringMaxText,
    "StreamChat.WidgetUtil.HashStringWithMax",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FHashStringMaxText::RunTest(const FString& Parameters)
{
    constexpr uint32 Max = 16;
    TMap<uint32, uint32> Histogram;
    for (char First = 'A'; First <= 'Z'; ++First)
    {
        for (char Second = 'A'; Second <= 'Z'; ++Second)
        {
            FString String;
            String.Reserve(2);
            String.AppendChar(First);
            String.AppendChar(Second);
            uint32 Hash = WidgetUtil::HashStringWithMax(String, Max);
            Histogram.FindOrAdd(Hash)++;
        }
    }
    Histogram.ValueSort(TLess<uint32>());
    constexpr float ExpectedFrequency = 26.f * 26.f / Max;
    constexpr uint32 MinFreq = static_cast<uint32>(0.8f * ExpectedFrequency);
    constexpr uint32 MaxFreq = static_cast<uint32>(1.2f * ExpectedFrequency);
    for (auto [Key, Value] : Histogram)
    {
        AddInfo(FString::Printf(TEXT("%d: %d"), Key, Value));
    }
    TestTrue(TEXT("Not too infrequent"), Histogram[0] > MinFreq);
    TestTrue(TEXT("Not too frequent"), Histogram[Max - 1] < MaxFreq);
    return true;
}
#endif
