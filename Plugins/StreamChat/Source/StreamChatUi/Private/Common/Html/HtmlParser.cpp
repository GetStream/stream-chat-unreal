// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlParser.h"

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

FHtmlScanner::FToken FHtmlScanner::ScanToken()
{
    SkipWhitespace();
    PrevStart = Start;
    Start = Current;

    if (IsAtEnd())
    {
        return FToken{ETokenType::Eof};
    }

    const TCHAR Char = Advance();
    if (bInTag)
    {
        switch (Char)
        {
            case TEXT('>'):
                bInTag = false;
                return MakeToken(ETokenType::AngleClose);
            case TEXT('/'):
                return MakeToken(ETokenType::Slash);
            case TEXT('='):
                return MakeToken(ETokenType::Equal);
            case TEXT('"'):
                return String();
            default:
            {
                if (FChar::IsAlpha(Char))
                {
                    return Identifier();
                }
                return MakeToken(ETokenType::Error);
            }
        }
    }
    if (Char == TEXT('<'))
    {
        bInTag = true;
        return MakeToken(ETokenType::AngleOpen);
    }

    // Backtrack. Content() needs to see advanced char
    Current--;
    return Content();
}

const FString& FHtmlScanner::GetOutput() const
{
    return Source;
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
        FStringView(Source).Mid(Start, Current - Start),
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

TCHAR Unescape(const FStringView& Str)
{
    switch (Str[0])
    {
        case TEXT('a'):
            switch (Str[1])
            {
                case TEXT('p'):
                    if (Str[2] == TEXT('o') && Str[3] == TEXT('s'))
                    {
                        return TEXT('\'');
                    }
                case TEXT('m'):
                    if (Str[2] == TEXT('p'))
                    {
                        return TEXT('&');
                    }
                default:
                    break;
            }
        case TEXT('q'):
            if (Str[1] == TEXT('u') && Str[2] == TEXT('o') && Str[3] == TEXT('t'))
            {
                return TEXT('"');
            }
        case TEXT('g'):
            if (Str[1] == TEXT('t'))
            {
                return TEXT('>');
            }
        case TEXT('l'):
            if (Str[1] == TEXT('t'))
            {
                return TEXT('<');
            }
        default:
            break;
    }
    return TEXT('?');
}

FHtmlScanner::FToken FHtmlScanner::Content()
{
    while (!IsAtEnd() && Peek() != TEXT('<') && Peek() != TEXT('>') && Peek() != TEXT('"') && Peek() != TEXT('\''))
    {
        // Unescape
        if (Peek() == TEXT('&'))
        {
            const int32 Amp = Current;
            int32 Len = 0;
            while (Advance() != TEXT(';'))
            {
                ++Len;
            }
            const FStringView Escaped = FStringView(Source).Mid(Amp + 1, Len);
            const TCHAR Unescaped = Unescape(Escaped);
            // Remove including leading ampersand
            Source.RemoveAt(Amp, Len + 1);
            Source.InsertAt(Amp, Unescaped);
            Current = Amp;
        }

        Advance();
    }

    // Allow for a single whitespace as the start of a content
    if (Start > 0 && FChar::IsWhitespace(Source[Start - 1]))
    {
        --Start;
    }
    return MakeToken(ETokenType::Content);
}

FHtmlParser::FHtmlParser(const FString& Source, FCallbackFn InCallback) : Callback(MoveTemp(InCallback)), Scanner(FHtmlScanner(Source))
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

    if (Current.Type == FHtmlScanner::ETokenType::Content)
    {
        // Fallback to display exact text
        return Content();
    }
    return false;
}

FTextRange FHtmlParser::GetContentRange() const
{
    if (Current.Type == FHtmlScanner::ETokenType::Content)
    {
        return {Scanner.Start, Scanner.Current};
    }
    return {};
}

FTextRange FHtmlParser::GetOriginalRange() const
{
    if (ElementStack.Num() == 0)
    {
        return GetContentRange();
    }
    return {ElementStack.Top().OpeningTagStart, Scanner.Current};
}

FStringView FHtmlParser::GetContent() const
{
    if (Current.Type == FHtmlScanner::ETokenType::Content)
    {
        return Current.Lexeme;
    }
    return {};
}

const FString& FHtmlParser::GetOutput() const
{
    return Scanner.GetOutput();
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
    // Parse start tag <a>
    check(Current.Type == FHtmlScanner::ETokenType::Identifier);
    if (FName(Current.Lexeme) == HtmlTag::LineBreak || FName(Current.Lexeme) == HtmlTag::HorizontalRule)
    {
        Newline(Scanner.PrevStart);
    }
    ElementStack.Push(FElement{Current.Lexeme});
    ElementStack.Top().OpeningTagStart = Scanner.PrevStart;
    Advance();

    // Parse attributes a="b" c="d"
    while (true)
    {
        if (!Attribute())
        {
            return false;
        }

        // Parse void elements <a/>
        if (AdvanceMatching(FHtmlScanner::ETokenType::Slash))
        {
            Callback(*this);
            if (!AdvanceMatching(FHtmlScanner::ETokenType::AngleClose))
            {
                return false;
            }
            CloseElement();
            return true;
        }

        if (AdvanceMatching(FHtmlScanner::ETokenType::AngleClose))
        {
            break;
        }
    }

    while (true)
    {
        // Inside an element we can either have content or other tags
        switch (Current.Type)
        {
            case FHtmlScanner::ETokenType::Content:
                if (!Content())
                {
                    return false;
                }
                break;
            case FHtmlScanner::ETokenType::AngleOpen:
                Advance();

                // Parse ending tag </a>
                if (AdvanceMatching(FHtmlScanner::ETokenType::Slash))
                {
                    if (!AdvanceMatching(FHtmlScanner::ETokenType::Identifier))
                    {
                        return false;
                    }
                    if (!AdvanceMatching(FHtmlScanner::ETokenType::AngleClose))
                    {
                        return false;
                    }
                    CloseElement();
                    return true;
                }

                // Parse child element
                if (!Element())
                {
                    return false;
                }
                break;
            default:
                return false;
        }
    }
}

void FHtmlParser::CloseElement()
{
    if (FName(ElementStack.Top().Name) == HtmlTag::Paragraph || FName(ElementStack.Top().Name) == HtmlTag::ListItem ||
        FName(ElementStack.Top().Name) == HtmlTag::HorizontalRule)
    {
        Newline(Scanner.Start);
    }
    ElementStack.Pop();
}

void FHtmlParser::Newline(const uint32 Index)
{
    ParagraphStartIndex = Index;
    ++Line;
}

bool FHtmlParser::Attribute()
{
    const FStringView Name = Current.Lexeme;
    if (AdvanceMatching(FHtmlScanner::ETokenType::Identifier))
    {
        if (!AdvanceMatching(FHtmlScanner::ETokenType::Equal))
        {
            return false;
        }
        const FStringView Value = Current.Lexeme;
        if (!AdvanceMatching(FHtmlScanner::ETokenType::String))
        {
            return false;
        }

        ElementStack.Last().Attributes.Add(Name, Value.LeftChop(1).RightChop(1));
    }
    return true;
}

bool FHtmlParser::Content()
{
    check(Current.Type == FHtmlScanner::ETokenType::Content);
    Callback(*this);
    Advance();
    return true;
}
