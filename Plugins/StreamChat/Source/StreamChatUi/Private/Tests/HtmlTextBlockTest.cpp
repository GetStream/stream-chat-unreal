// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "CoreTypes.h"
#include "Message/HtmlTextBlock.h"
#include "Misc/AutomationTest.h"
#include "WidgetUtil.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FHtmlTextBlockTest,
    "StreamChat.Html.Parse",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FHtmlTextBlockTest::RunTest(const FString& Parameters)
{
    // Simple
    {
        const FString Source{TEXT("<p>Hi</p>")};
        FHtmlParser Parser(
            Source,
            [&](const FStringView& Content, const TArray<FHtmlParser::FElement>& ElementStack)
            {
                TestTrue("Content", Content.Equals(TEXT("Hi")));
                TestEqual("ElementStackSize", ElementStack.Num(), 1);
                TestTrue("p", ElementStack[0].Name.Equals(TEXT("p")));
            });
        TestTrue("Parsed", Parser.Parse());
    }
    // Nested
    {
        const FString Source{TEXT("<p>a<em>b</em>c</p>")};
        int32 Iteration = 0;
        FHtmlParser Parser(
            Source,
            [&](const FStringView& Content, const TArray<FHtmlParser::FElement>& ElementStack)
            {
                switch (Iteration)
                {
                    case 0:
                        TestTrue("Content", Content.Equals(TEXT("a")));
                        TestEqual("ElementStackSize", ElementStack.Num(), 1);
                        TestTrue("ElementStack", ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", ElementStack[0].Attributes.Num(), 0);
                        break;
                    case 1:
                        TestTrue("Content", Content.Equals(TEXT("b")));
                        TestEqual("ElementStackSize", ElementStack.Num(), 2);
                        TestTrue("ElementStack[0]", ElementStack[0].Name.Equals(TEXT("p")));
                        TestTrue("ElementStack[1]", ElementStack[1].Name.Equals(TEXT("em")));
                        TestEqual("Attribs[0]", ElementStack[0].Attributes.Num(), 0);
                        TestEqual("Attribs[1]", ElementStack[1].Attributes.Num(), 0);
                        break;
                    case 2:
                        TestTrue("Content", Content.Equals(TEXT("c")));
                        TestEqual("ElementStackSize", ElementStack.Num(), 1);
                        TestTrue("ElementStack", ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", ElementStack[0].Attributes.Num(), 0);
                        break;
                    default:
                        break;
                }
                ++Iteration;
            });
        TestTrue("Parsed", Parser.Parse());
    }
    // Attribute
    {
        const FString Source{TEXT("<a href=\"https://getstream.io\">Hi</p>")};
        FHtmlParser Parser(
            Source,
            [&](const FStringView& Content, const TArray<FHtmlParser::FElement>& ElementStack)
            {
                TestTrue("Content", Content.Equals(TEXT("Hi")));
                TestEqual("ElementStackSize", ElementStack.Num(), 1);
                TestTrue("a", ElementStack[0].Name.Equals(TEXT("a")));
                TestTrue("href", ElementStack[0].Attributes[TEXT("href")].Equals(TEXT("https://getstream.io")));
            });
        TestTrue("Parsed", Parser.Parse());
    }
    return true;
}
#endif
