// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Message/HtmlTextBlock.h"

TSharedRef<FHtmlRichTextMarkupParser> FHtmlRichTextMarkupParser::GetStaticInstance()
{
    static TSharedRef<FHtmlRichTextMarkupParser> Parser = MakeShareable(new FHtmlRichTextMarkupParser());
    return Parser;
}

void FHtmlRichTextMarkupParser::Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output)
{
    FHtmlParser Parser(
        Input,
        [](const FStringView& Content, const TArray<FHtmlParser::FElement>& ElementStack) {

        });
    Parser.Parse();
}

FHtmlRichTextMarkupParser::FHtmlRichTextMarkupParser()
{
}

FHtmlScanner::FHtmlScanner(const FString& InSource) : Source(InSource)
{
}

TCHAR FHtmlScanner::Advance()
{
    return Source[Current++];
}

void FHtmlScanner::SkipWhitespace()
{
    while (!IsAtEnd())
    {
        if (FChar::IsWhitespace(Peek()))
        {
            Advance();
        }
        // Skip HTML Comments
        else if (
            Source.Len() - Current > 6 && Peek() == TEXT('<') && Source[Current + 1] == TEXT('!') && Source[Current + 2] == TEXT('-') &&
            Source[Current + 3] == TEXT('-'))
        {
            while (!IsAtEnd() && Source.Len() - Current > 2 && Peek() == TEXT('-') && Source[Current + 1] == TEXT('-') && Source[Current + 2] == TEXT('>'))
            {
                Advance();
            }
        }
        else
        {
            return;
        }
    }
}

FHtmlScanner ::FToken FHtmlScanner::ScanToken()
{
    SkipWhitespace();
    Start = Current;

    if (IsAtEnd())
    {
        return FToken{ETokenType::Eof};
    }

    switch (const TCHAR Char = Advance())
    {
        case TEXT('<'):
            bInElement = true;
            return MakeToken(ETokenType::AngleOpen);
        case TEXT('>'):
            bInElement = false;
            return MakeToken(ETokenType::AngleClose);
        case TEXT('/'):
            return MakeToken(ETokenType::Slash);
        case TEXT('='):
            return MakeToken(ETokenType::Equal);
        case TEXT('"'):
            return String();
        default:
        {
            if (!bInElement)
            {
                return Content();
            }
            if (FChar::IsAlpha(Char))
            {
                return Identifier();
            }
            return MakeToken(ETokenType::Error);
        }
    }
}

bool FHtmlScanner::IsAtEnd() const
{
    return Current == Source.Len();
}

TCHAR FHtmlScanner::Peek() const
{
    return Source[Current];
}

FHtmlScanner::FToken FHtmlScanner::MakeToken(const ETokenType Type) const
{
    return FToken{
        Type,
        Source.Mid(Start, Current - Start),
    };
}

FHtmlScanner::FToken FHtmlScanner::String()
{
    while (Peek() != TEXT('"') && !IsAtEnd())
    {
        Advance();
    }

    if (IsAtEnd())
    {
        // Unterminated string
        return MakeToken(ETokenType::Error);
    }

    // The closing quote
    Advance();
    return MakeToken(ETokenType::String);
}

FHtmlScanner::FToken FHtmlScanner::Identifier()
{
    while (FChar::IsAlnum(Peek()) && !IsAtEnd())
    {
        Advance();
    }
    return MakeToken(ETokenType::Identifier);
}

FHtmlScanner::FToken FHtmlScanner::Content()
{
    while (!IsAtEnd() && Peek() != TEXT('<') && Peek() != TEXT('>') && Peek() != TEXT('"') && Peek() != TEXT('\''))
    {
        Advance();
    }
    return MakeToken(ETokenType::Content);
}

FHtmlParser::FHtmlParser(const FString& Source, TFunctionRef<void(const FStringView& Content, const TArray<FElement>& ElementStack)> InCallback)
    : Callback(MoveTemp(InCallback)), Scanner(FHtmlScanner(Source))
{
    // Prime
    Advance();
}

bool FHtmlParser::Parse()
{
    // Should only have one root element
    if (AdvanceMatching(FHtmlScanner::ETokenType::AngleOpen))
    {
        return Element();
    }

    return false;
}

void FHtmlParser::Advance()
{
    while (true)
    {
        Current = Scanner.ScanToken();
        if (Current.Type != FHtmlScanner::ETokenType::Error)
        {
            break;
        }
        UE_LOG(LogTemp, Error, TEXT("Error parsing HTML"));
    }
}

bool FHtmlParser::AdvanceMatching(const FHtmlScanner::ETokenType TokenType)
{
    if (Current.Type != TokenType)
    {
        return false;
    }
    Advance();
    return true;
}

bool FHtmlParser::Element()
{
    check(Current.Type == FHtmlScanner::ETokenType::Identifier);
    ElementStack.Push(FElement{Current.Lexeme});
    Advance();

    // Parse attributes
    while (true)
    {
        if (!Attribute())
        {
            return false;
        }
        if (AdvanceMatching(FHtmlScanner::ETokenType::AngleClose))
        {
            break;
        }
    }

    // Parse content
    if (Current.Type == FHtmlScanner::ETokenType::Content)
    {
        if (!Content())
        {
            return false;
        }
    }

    if (AdvanceMatching(FHtmlScanner::ETokenType::AngleOpen))
    {
        // Parse ending tag
        if (AdvanceMatching(FHtmlScanner::ETokenType::Slash))
        {
            if (!AdvanceMatching(FHtmlScanner::ETokenType::Identifier))
            {
                return false;
            }
            ElementStack.Pop();
            if (!AdvanceMatching(FHtmlScanner::ETokenType::AngleClose))
            {
                return false;
            }
        }
        else
        {
            // Parse child element
            if (!Element())
            {
                return false;
            }

            // Parse content after child element
            if (Current.Type == FHtmlScanner::ETokenType::Content)
            {
                if (!Content())
                {
                    return false;
                }
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool FHtmlParser::Attribute()
{
    const FHtmlScanner::FToken Name = Current;
    if (AdvanceMatching(FHtmlScanner::ETokenType::Identifier))
    {
        if (!AdvanceMatching(FHtmlScanner::ETokenType::Equal))
        {
            return false;
        }
        const FHtmlScanner::FToken Value = Current;
        if (!AdvanceMatching(FHtmlScanner::ETokenType::String))
        {
            return false;
        }

        ElementStack.Last().Attributes.Add(Name.Lexeme, Value.Lexeme.LeftChop(1).RightChop(1));
    }
    return true;
}

bool FHtmlParser::Content()
{
    check(Current.Type == FHtmlScanner::ETokenType::Content);
    Callback(Current.Lexeme, ElementStack);
    Advance();
    return true;
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

// void FHtmlScanner::Parse()
// {
//     if (Input.Len() == 0)
//     {
//         return;
//     }
//
//     FText ErrorMsg;
//     int32 ErrorLine = 0;
//     if (!FFastXml::ParseXmlFile(this, nullptr, InputPtr, nullptr, false, false, ErrorMsg, ErrorLine))
//     {
//     }
//
//     if (LastResult().Range.EndIndex == INDEX_NONE)
//     {
//         LastResult().Range.EndIndex = LastRun().OriginalRange.EndIndex;
//     }
// }
//
// bool FHtmlScanner::ProcessXmlDeclaration(const TCHAR* ElementData, int32 XmlFileLineNumber)
// {
//     UE_LOG(LogTemp, Log, TEXT("%s"), ElementData);
//     return true;
// }
//
// bool FHtmlScanner::ProcessElement(const TCHAR* ElementName, const TCHAR* ElementData, int32 XmlFileLineNumber)
// {
//     // -1 to include <
//     const int32 StartIndex = GetStartIndex(ElementName) - 1;
//     const FTextRange ElementRange{StartIndex, INDEX_NONE};
//     const bool bIsP = FCString::Strcmp(ElementName, TEXT("p")) == 0;
//     if (bIsP)
//     {
//         LineParseResults.Add(FTextLineParseResults{ElementRange});
//     }
//     else
//     {
//         ElementStack.Push(ElementName);
//     }
//
//     const FString RunName = [&]() -> FString
//     {
//         // Generate combination of outer elements
//         TArray RunNames{ElementStack};
//         RunNames.Sort();
//         return FString::Join(RunNames, TEXT("_"));
//     }();
//
//     // Overwrite run name if empty
//     if (LastResult().Runs.Num() > 0 && LastRun().ContentRange.IsEmpty() && LastRun().OriginalRange.EndIndex == INDEX_NONE)
//     {
//         LastRun().Name = RunName;
//     }
//     else
//     {
//         // Close last run
//         if (LastResult().Runs.Num() > 0)
//         {
//             const int32 End = GetEndIndex(ElementName);
//             LastRun().ContentRange.EndIndex = End;
//             LastRun().OriginalRange.EndIndex = End;
//         }
//         LastResult().Runs.Add(FTextRunParseResults{RunName, ElementRange});
//     }
//
//     if (ElementData)
//     {
//         LastRun().ContentRange = FTextRange{GetStartIndex(ElementData), GetEndIndex(ElementData)};
//         Output.Append(ElementData);
//     }
//
//     return true;
// }
//
// bool FHtmlScanner::ProcessAttribute(const TCHAR* AttributeName, const TCHAR* AttributeValue)
// {
//     UE_LOG(LogTemp, Log, TEXT("%s: %s"), AttributeName, AttributeValue);
//     return true;
// }
//
// bool FHtmlScanner::ProcessClose(const TCHAR* Element)
// {
//     // +1 to include >
//     const int32 End = GetEndIndex(Element) + 1;
//     LastRun().OriginalRange.EndIndex = End;
//     if (FCString::Strcmp(Element, TEXT("p")) == 0)
//     {
//         LastResult().Range.EndIndex = End;
//     }
//     else
//     {
//         ElementStack.Pop();
//     }
//     return true;
// }
//
// bool FHtmlScanner::ProcessComment(const TCHAR* Comment)
// {
//     return true;
// }
//
// int32 FHtmlScanner::GetStartIndex(const TCHAR* Str) const
// {
//     return Str - InputPtr;
// }
//
// int32 FHtmlScanner::GetEndIndex(const TCHAR* Str) const
// {
//     return GetStartIndex(Str) + FCString::Strlen(Str);
// }
//
// FTextRunParseResults& FHtmlScanner::LastRun()
// {
//     return LastResult().Runs.Last();
// }
//
// FTextLineParseResults& FHtmlScanner::LastResult()
// {
//     if (LineParseResults.Num() == 0)
//     {
//         LineParseResults.Add(FTextLineParseResults{{0, INDEX_NONE}});
//     }
//     return LineParseResults.Last();
// }

TSharedPtr<IRichTextMarkupParser> UHtmlTextBlock::CreateMarkupParser()
{
    return FHtmlRichTextMarkupParser::GetStaticInstance();
}

TSharedPtr<IRichTextMarkupWriter> UHtmlTextBlock::CreateMarkupWriter()
{
    return FHtmlRichTextMarkupWriter::GetStaticInstance();
}
