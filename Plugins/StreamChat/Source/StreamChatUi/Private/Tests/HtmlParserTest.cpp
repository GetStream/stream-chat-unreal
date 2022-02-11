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
            [&](const FHtmlParser& Self)
            {
                TestTrue("Content", Self.GetContent().Equals(TEXT("Hi")));
                TestEqual("ContentRangeBegin", Self.GetContentRange().BeginIndex, 3);
                TestEqual("ContentRangeEnd", Self.GetContentRange().EndIndex, 5);
                TestEqual("OriginalRangeBegin", Self.GetOriginalRange().BeginIndex, 0);
                TestEqual("OriginalRangeEnd", Self.GetOriginalRange().EndIndex, 5);
                TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                TestTrue("p", Self.ElementStack[0].Name.Equals(TEXT("p")));
            });
        TestTrue("Parsed", Parser.Parse());
    }
    // Unescape
    {
        const FString Source{TEXT("<p>&lt; hi &gt;</p>")};
        FHtmlParser Parser(
            Source,
            [&](const FHtmlParser& Self)
            {
                TestTrue("Content", Self.GetContent().Equals(TEXT("< hi >")));
                TestEqual("ContentRangeBegin", Self.GetContentRange().BeginIndex, 3);
                TestEqual("ContentRangeEnd", Self.GetContentRange().EndIndex, 9);
                TestEqual("OriginalRangeBegin", Self.GetOriginalRange().BeginIndex, 0);
                TestEqual("OriginalRangeEnd", Self.GetOriginalRange().EndIndex, 9);
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
            [&](const FHtmlParser& Self)
            {
                switch (Iteration)
                {
                    case 0:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("a")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    case 1:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("b")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 2);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestTrue("ElementStack[1]", Self.ElementStack[1].Name.Equals(TEXT("em")));
                        TestEqual("Attribs[0]", Self.ElementStack[0].Attributes.Num(), 0);
                        TestEqual("Attribs[1]", Self.ElementStack[1].Attributes.Num(), 0);
                        break;
                    case 2:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("c")));
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
            [&](const FHtmlParser& Self)
            {
                TestTrue("Content", Self.GetContent().Equals(TEXT("Hi")));
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
            [&](const FHtmlParser& Self)
            {
                TestTrue("Content", Self.GetContent().IsEmpty());
                TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                TestTrue("img", Self.ElementStack[0].Name.Equals(TEXT("img")));
                TestTrue("src", Self.ElementStack[0].Attributes[TEXT("src")].Equals(TEXT("img_girl.jpg")));
                TestTrue("alt", Self.ElementStack[0].Attributes[TEXT("alt")].Equals(TEXT("Girl in a jacket")));
                TestTrue("width", Self.ElementStack[0].Attributes[TEXT("width")].Equals(TEXT("500")));
                TestTrue("height", Self.ElementStack[0].Attributes[TEXT("height")].Equals(TEXT("600")));
            });
        TestTrue("Parsed", Parser.Parse());
    }

    // Nested void elements
    {
        const FString Source{TEXT("<p><strong>Hi</strong><br/><br/>friend</p>")};
        int32 Iteration = 0;
        FHtmlParser Parser(
            Source,
            [&](const FHtmlParser& Self)
            {
                switch (Iteration)
                {
                    case 0:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("Hi")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 2);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestTrue("ElementStack[1]", Self.ElementStack[1].Name.Equals(TEXT("strong")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    case 1:
                    case 2:
                        TestTrue("Content", Self.GetContent().IsEmpty());
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 2);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestTrue("ElementStack[1]", Self.ElementStack[1].Name.Equals(TEXT("br")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    case 3:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("friend")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    default:
                        break;
                }
                ++Iteration;
            });
        TestTrue("Parsed", Parser.Parse());
    }

    // Spacing
    {
        const FString Source{TEXT("<p><del>no way</del> yes way</p>")};
        int32 Iteration = 0;
        FHtmlParser Parser(
            Source,
            [&](const FHtmlParser& Self)
            {
                switch (Iteration)
                {
                    case 0:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("no way")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 2);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestTrue("ElementStack[1]", Self.ElementStack[1].Name.Equals(TEXT("del")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    case 1:
                        TestTrue("Content", Self.GetContent().Equals(TEXT(" yes way")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    default:
                        break;
                }
                ++Iteration;
            });
        TestTrue("Parsed", Parser.Parse());
    }

    // Equals signs
    {
        const FString Source{TEXT("<p>==very important== yep</p>")};
        FHtmlParser Parser(
            Source,
            [&](const FHtmlParser& Self)
            {
                TestTrue("Content", Self.GetContent().Equals(TEXT("==very important== yep")));
                TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                TestTrue("p", Self.ElementStack[0].Name.Equals(TEXT("p")));
            });
        TestTrue("Parsed", Parser.Parse());
    }

    // Newline
    {
        const FString Source{TEXT("<p>A<br/>B</p>")};
        int32 Iteration = 0;
        FHtmlParser Parser(
            Source,
            [&](const FHtmlParser& Self)
            {
                switch (Iteration)
                {
                    case 0:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("A")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    case 1:
                        TestTrue("Content", Self.GetContent().IsEmpty());
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 2);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestTrue("ElementStack[0]", Self.ElementStack[1].Name.Equals(TEXT("br")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        TestEqual("Attribs", Self.ElementStack[1].Attributes.Num(), 0);
                        break;
                    case 2:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("B")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        break;
                    default:
                        break;
                }
                ++Iteration;
            });
        TestTrue("Parsed", Parser.Parse());
    }

    // Unescape
    {
        const FString Source{TEXT("<p>A<br/><br/>&gt; B<br/><br/>C</p>")};
        int32 Iteration = 0;
        FHtmlParser Parser(
            Source,
            [&](const FHtmlParser& Self)
            {
                switch (Iteration)
                {
                    case 0:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("A")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        TestEqual("OpeningTagStart", Self.ElementStack[0].OpeningTagStart, 0);
                        break;
                    case 3:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("> B")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        TestEqual("OpeningTagStart", Self.ElementStack[0].OpeningTagStart, 0);
                        break;
                    case 6:
                        TestTrue("Content", Self.GetContent().Equals(TEXT("C")));
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 1);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        TestEqual("OpeningTagStart", Self.ElementStack[0].OpeningTagStart, 0);
                        break;
                    case 1:
                    case 2:
                    case 4:
                    case 5:
                        TestTrue("Content", Self.GetContent().IsEmpty());
                        TestEqual("ElementStackSize", Self.ElementStack.Num(), 2);
                        TestTrue("ElementStack[0]", Self.ElementStack[0].Name.Equals(TEXT("p")));
                        TestTrue("ElementStack[1]", Self.ElementStack[1].Name.Equals(TEXT("br")));
                        TestEqual("Attribs", Self.ElementStack[0].Attributes.Num(), 0);
                        TestEqual("Attribs", Self.ElementStack[1].Attributes.Num(), 0);
                        if (Iteration == 1)
                        {
                            TestEqual("OpeningTagStart", Self.ElementStack[1].OpeningTagStart, 4);
                        }
                        if (Iteration == 2)
                        {
                            TestEqual("OpeningTagStart", Self.ElementStack[1].OpeningTagStart, 9);
                        }
                        if (Iteration == 4)
                        {
                            TestEqual("OpeningTagStart", Self.ElementStack[1].OpeningTagStart, 17);
                        }
                        if (Iteration == 5)
                        {
                            TestEqual("OpeningTagStart", Self.ElementStack[1].OpeningTagStart, 22);
                        }
                        break;
                    default:
                        break;
                }
                ++Iteration;
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
    // Newline
    {
        FString Output;
        const FString Input{TEXT("<p>A<br/>B</p>")};
        TArray<FTextLineParseResults> Results;
        Parser->Process(Results, Input, Output);
        TestEqual("Output", Output, Input);
        TestEqual("Results", Results.Num(), 2);
        TestEqual("Results[0].Begin", Results[0].Range.BeginIndex, 0);
        TestEqual("Results[0].End", Results[0].Range.EndIndex, 4);
        TestEqual("Results[0].Runs", Results[0].Runs.Num(), 1);
        TestEqual("Results[0].RunName", Results[0].Runs[0].Name, TEXT("p"));
        TestEqual("Results[0].RunOriginalStart", Results[0].Runs[0].OriginalRange.BeginIndex, 0);
        TestEqual("Results[0].RunOriginalEnd", Results[0].Runs[0].OriginalRange.EndIndex, 4);
        TestEqual("Results[0].RunContentStart", Results[0].Runs[0].ContentRange.BeginIndex, 3);
        TestEqual("Results[0].RunContentEnd", Results[0].Runs[0].ContentRange.EndIndex, 4);
        TestEqual("Results[1].Begin", Results[1].Range.BeginIndex, 4);
        TestEqual("Results[1].End", Results[1].Range.EndIndex, 14);
        TestEqual("Results[1].Runs", Results[1].Runs.Num(), 2);
        TestEqual("Results[1].Run[0]Name", Results[1].Runs[0].Name, TEXT("p_br"));
        TestEqual("Results[1].Run[0]OriginalStart", Results[1].Runs[0].OriginalRange.BeginIndex, 4);
        TestEqual("Results[1].Run[0]OriginalEnd", Results[1].Runs[0].OriginalRange.EndIndex, 9);
        TestEqual("Results[1].Run[0]ContentStart", Results[1].Runs[0].ContentRange.BeginIndex, INDEX_NONE);
        TestEqual("Results[1].Run[0]ContentEnd", Results[1].Runs[0].ContentRange.EndIndex, INDEX_NONE);
        TestEqual("Results[1].Run[1]Name", Results[1].Runs[1].Name, TEXT("p"));
        TestEqual("Results[1].Run[1]OriginalStart", Results[1].Runs[1].OriginalRange.BeginIndex, 0);
        TestEqual("Results[1].Run[1]OriginalEnd", Results[1].Runs[1].OriginalRange.EndIndex, 10);
        TestEqual("Results[1].Run[1]ContentStart", Results[1].Runs[1].ContentRange.BeginIndex, 9);
        TestEqual("Results[1].Run[1]ContentEnd", Results[1].Runs[1].ContentRange.EndIndex, 10);
    }
    // Newline
    {
        FString Output;
        const FString Input{TEXT("<p>A<br/>&gt; B<br/>C</p>")};
        TArray<FTextLineParseResults> Results;
        Parser->Process(Results, Input, Output);
        TestEqual("Output", Output, TEXT("<p>A<br/>> B<br/>C</p>"));
        TestEqual("Results", Results.Num(), 3);
        TestEqual("Results[0].Begin", Results[0].Range.BeginIndex, 0);
        TestEqual("Results[0].End", Results[0].Range.EndIndex, 4);
        TestEqual("Results[1].Begin", Results[1].Range.BeginIndex, 4);
        TestEqual("Results[1].End", Results[1].Range.EndIndex, 12);
        TestEqual("Results[2].Begin", Results[2].Range.BeginIndex, 12);
        TestEqual("Results[2].End", Results[2].Range.EndIndex, 22);
    }
    return true;
}
#endif
