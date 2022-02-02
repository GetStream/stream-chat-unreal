// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Message/HtmlTextBlock.h"

#include "FastXml.h"

TSharedRef<FHtmlRichTextMarkupParser> FHtmlRichTextMarkupParser::GetStaticInstance()
{
    static TSharedRef<FHtmlRichTextMarkupParser> Parser = MakeShareable(new FHtmlRichTextMarkupParser());
    return Parser;
}

void FHtmlRichTextMarkupParser::Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output)
{
    FHtmlParser Parser(Input);
    Parser.Parse();

    Results = Parser.LineParseResults;
    Output = Parser.Output;
}

FHtmlRichTextMarkupParser::FHtmlRichTextMarkupParser()
{
}

TSharedRef<FHtmlRichTextMarkupWriter> FHtmlRichTextMarkupWriter::GetStaticInstance()
{
    static TSharedRef<FHtmlRichTextMarkupWriter> Parser = MakeShareable(new FHtmlRichTextMarkupWriter());
    return Parser;
}

void FHtmlRichTextMarkupWriter::Write(const TArray<FRichTextLine>& InLines, FString& Output)
{
}

FHtmlRichTextMarkupWriter::FHtmlRichTextMarkupWriter()
{
}

FHtmlParser::FHtmlParser(const FString& InInput) : Input(InInput), InputPtr(const_cast<TCHAR*>(*Input))
{
}

void FHtmlParser::Parse()
{
    if (Input.Len() == 0)
    {
        return;
    }

    FText ErrorMsg;
    int32 ErrorLine = 0;
    if (!FFastXml::ParseXmlFile(this, nullptr, InputPtr, nullptr, false, false, ErrorMsg, ErrorLine))
    {
    }

    if (LastResult().Range.EndIndex == INDEX_NONE)
    {
        LastResult().Range.EndIndex = LastRun().OriginalRange.EndIndex;
    }
}

bool FHtmlParser::ProcessXmlDeclaration(const TCHAR* ElementData, int32 XmlFileLineNumber)
{
    UE_LOG(LogTemp, Log, TEXT("%s"), ElementData);
    return true;
}

bool FHtmlParser::ProcessElement(const TCHAR* ElementName, const TCHAR* ElementData, int32 XmlFileLineNumber)
{
    // -1 to include <
    const int32 StartIndex = GetStartIndex(ElementName) - 1;
    const FTextRange ElementRange{StartIndex, INDEX_NONE};
    const bool bIsP = FCString::Strcmp(ElementName, TEXT("p")) == 0;
    if (bIsP)
    {
        LineParseResults.Add(FTextLineParseResults{ElementRange});
    }
    else
    {
        ElementStack.Push(ElementName);
    }

    const FString RunName = [&]() -> FString
    {
        // Generate combination of outer elements
        TArray RunNames{ElementStack};
        RunNames.Sort();
        return FString::Join(RunNames, TEXT("_"));
    }();

    // Overwrite run name if empty
    if (LastResult().Runs.Num() > 0 && LastRun().ContentRange.IsEmpty() && LastRun().OriginalRange.EndIndex == INDEX_NONE)
    {
        LastRun().Name = RunName;
    }
    else
    {
        // Close last run
        if (LastResult().Runs.Num() > 0)
        {
            const int32 End = GetEndIndex(ElementName);
            LastRun().ContentRange.EndIndex = End;
            LastRun().OriginalRange.EndIndex = End;
        }
        LastResult().Runs.Add(FTextRunParseResults{RunName, ElementRange});
    }

    if (ElementData)
    {
        LastRun().ContentRange = FTextRange{GetStartIndex(ElementData), GetEndIndex(ElementData)};
        Output.Append(ElementData);
    }

    return true;
}

bool FHtmlParser::ProcessAttribute(const TCHAR* AttributeName, const TCHAR* AttributeValue)
{
    UE_LOG(LogTemp, Log, TEXT("%s: %s"), AttributeName, AttributeValue);
    return true;
}

bool FHtmlParser::ProcessClose(const TCHAR* Element)
{
    // +1 to include >
    const int32 End = GetEndIndex(Element) + 1;
    LastRun().OriginalRange.EndIndex = End;
    if (FCString::Strcmp(Element, TEXT("p")) == 0)
    {
        LastResult().Range.EndIndex = End;
    }
    else
    {
        ElementStack.Pop();
    }
    return true;
}

bool FHtmlParser::ProcessComment(const TCHAR* Comment)
{
    return true;
}

int32 FHtmlParser::GetStartIndex(const TCHAR* Str) const
{
    return Str - InputPtr;
}

int32 FHtmlParser::GetEndIndex(const TCHAR* Str) const
{
    return GetStartIndex(Str) + FCString::Strlen(Str);
}

FTextRunParseResults& FHtmlParser::LastRun()
{
    return LastResult().Runs.Last();
}

FTextLineParseResults& FHtmlParser::LastResult()
{
    if (LineParseResults.Num() == 0)
    {
        LineParseResults.Add(FTextLineParseResults{{0, INDEX_NONE}});
    }
    return LineParseResults.Last();
}

TSharedPtr<IRichTextMarkupParser> UHtmlTextBlock::CreateMarkupParser()
{
    return FHtmlRichTextMarkupParser::GetStaticInstance();
}

TSharedPtr<IRichTextMarkupWriter> UHtmlTextBlock::CreateMarkupWriter()
{
    return FHtmlRichTextMarkupWriter::GetStaticInstance();
}
