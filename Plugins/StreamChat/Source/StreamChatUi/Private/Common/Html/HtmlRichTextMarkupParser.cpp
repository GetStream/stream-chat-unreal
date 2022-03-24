// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlRichTextMarkupParser.h"

#include "Algo/Transform.h"
#include "Common/Html/HtmlParser.h"

namespace
{
TMap<FString, FTextRange> GenerateMetaData(const TArray<FHtmlParser::FElement>& ElementStack, const FString& OutputString)
{
    TMap<FString, FTextRange> MetaData;
    if (ElementStack.Num() > 0)
    {
        for (const FHtmlParser::FElement& Elem : ElementStack)
        {
            for (const auto& Attrib : Elem.Attributes)
            {
                const int32 BeginIndex = static_cast<int32>(Attrib.Value.GetData() - *OutputString);
                const int32 EndIndex = BeginIndex + Attrib.Value.Len();
                const FTextRange Range{BeginIndex, EndIndex};
                MetaData.Add(FString{Attrib.Key}, Range);
            }
        }
    }
    return MetaData;
}

FString MakeRunName(const TArray<FHtmlParser::FElement>& ElementStack)
{
    TSet<FStringView> ElementNames;
    Algo::Transform(ElementStack, ElementNames, [](auto&& Element) { return Element.Name; });
    return FString::Join(ElementNames, TEXT("_"));
}
}    // namespace

TSharedRef<FHtmlRichTextMarkupParser> FHtmlRichTextMarkupParser::GetStaticInstance()
{
    static TSharedRef<FHtmlRichTextMarkupParser> Parser = MakeShareable(new FHtmlRichTextMarkupParser());
    return Parser;
}

void FHtmlRichTextMarkupParser::Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output)
{
    if (Input.IsEmpty())
    {
        return;
    }

    FHtmlParser Parser(
        Input,
        [&Results](const FHtmlParser& Self)
        {
            while (!Results.IsValidIndex(Self.Line))
            {
                Results.Add(FTextLineParseResults{{Self.ParagraphStartIndex, INDEX_NONE}});
                if (Results.Num() > 1)
                {
                    Results.Last(1).Range.EndIndex = Self.ParagraphStartIndex;
                }
            }

            // <br/> should only create line break. No content.
            if (Self.ElementStack.Num() > 0 && FName(Self.ElementStack.Top().Name) == HtmlTag::LineBreak)
            {
                return;
            }

            const FString Name = MakeRunName(Self.ElementStack);
            FTextRunParseResults Run{Name, Self.GetOriginalRange(), Self.GetContentRange()};
            Run.MetaData = GenerateMetaData(Self.ElementStack, Self.GetOutput());

            Results[Self.Line].Runs.Add(Run);
        });

    const bool bSuccess = Parser.Parse();
    if (ensure(bSuccess))
    {
        Output = Parser.GetOutput();
        Results.Last().Range.EndIndex = Output.Len();
    }
}
