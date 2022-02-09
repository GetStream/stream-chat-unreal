// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlParser.h"
#include "Common/Html/HtmlTextBlock.h"
#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "WidgetUtil.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FHtmlParserTest,
    "StreamChat.Html.HtmlParser",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FHtmlParserTest::RunTest(const FString& Parameters)
{
    // Simple
    {
        const FString Source{TEXT("<p>Hi</p>")};
        FHtmlParser Parser(
            Source,
            [&](const FStringView& Content, const FHtmlParser& Self)
            {
                TestTrue("Content", Content.Equals(TEXT("Hi")));
                TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                TestTrue("p", Self.ElementStack[0].Name.Equals(TEXT("p")));
            });
        TestTrue("Parsed", Parser.Parse());
    }
    // Nested
    {
        const FString Source{TEXT("<p>a<em>b</em>c</p>")};
        int32 Iteration = 0;
        FHtmlParser Parser(
            Source,
            [&](const FStringView& Content, const FHtmlParser& Self)
            {
                switch (Iteration)
                {
                    case 0:
                        TestTrue("Content", Content.Equals(TEXT("a")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    case 1:
                        TestTrue("Content", Content.Equals(TEXT("b")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 2);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestTrue("ElementStack[1]", Self.ElementStack[1].Name.Equals(TEXT("em")));
                        TestEqual("Attribs[0]", Self.ElementStack[0].Attributes.Num(), 0);
                        TestEqual("Attribs[1]", Self.ElementStack[1].Attributes.Num(), 0);
                        break;
                    case 2:
                        TestTrue("Content", Content.Equals(TEXT("c")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
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
        const FString Source{TEXT("<a href=\"https://getstream.io\">Hi</a>")};
        FHtmlParser Parser(
            Source,
            [&](const FStringView& Content, const FHtmlParser& Self)
            {
                TestTrue("Content", Content.Equals(TEXT("Hi")));
                TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                TestTrue("a", Self.ElementStack[0].Name.Equals(TEXT("a")));
                TestTrue("href", Self.ElementStack[0].Attributes[TEXT("href")].Equals(TEXT("https://getstream.io")));
            });
        TestTrue("Parsed", Parser.Parse());
    }
    // Void elements
    {
        const FString Source{TEXT("<img src=\"img_girl.jpg\" alt=\"Girl in a jacket\" width=\"500\" height=\"600\"/>")};
        FHtmlParser Parser(
            Source,
            [&](const FStringView& Content, const FHtmlParser& Self)
            {
                TestTrue("Content", Content.IsEmpty());
                TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                TestTrue("img", Self.ElementStack[0].Name.Equals(TEXT("img")));
                TestTrue("src", Self.ElementStack[0].Attributes[TEXT("src")].Equals(TEXT("img_girl.jpg")));
                TestTrue("alt", Self.ElementStack[0].Attributes[TEXT("alt")].Equals(TEXT("Girl in a jacket")));
                TestTrue("width", Self.ElementStack[0].Attributes[TEXT("width")].Equals(TEXT("500")));
                TestTrue("height", Self.ElementStack[0].Attributes[TEXT("height")].Equals(TEXT("600")));
            });
        TestTrue("Parsed", Parser.Parse());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FHtmlRichTextMarkupParserTest,
    "StreamChat.Html.RichTextMarkupParser",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FHtmlRichTextMarkupParserTest::RunTest(const FString& Parameters)
{
    const TSharedRef<FHtmlRichTextMarkupParser> Parser = FHtmlRichTextMarkupParser::GetStaticInstance();
    // Simple
    {
        FString Output;
        const FString Input{TEXT("<p>Hi</p>")};
        TArray<FTextLineParseResults> Results;
        Parser->Process(Results, Input, Output);
    }
    return true;
}
#endif