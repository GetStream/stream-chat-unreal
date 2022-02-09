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
    return Content();
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
    if (Current.Lexeme.Equals(LineBreakTag))
    {
        Newline();
    }
    ElementStack.Push(FElement{Current.Lexeme});
    Advance();

    // Parse attributes a="b" c="d"
    while (true)
    {
        if (!Attribute())
        {
            return false;
        }
        if (AdvanceMatching(FHtmlScanner::ETokenType::Slash))
        {
            // Parse void elements <a/>
            return EmptyContent();
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
    if (ElementStack.Last().Name.Equals(ParagraphTag))
    {
        Newline();
    }
    ElementStack.Pop();
}

void FHtmlParser::Newline()
{
    ParagraphEndIndex = ParagraphStartIndex;
    ParagraphStartIndex = Scanner.Start;
    ++Line;
}

bool FHtmlParser::Attribute()
{
    const auto [NameType, Name] = Current;
    if (AdvanceMatching(FHtmlScanner::ETokenType::Identifier))
    {
        if (!AdvanceMatching(FHtmlScanner::ETokenType::Equal))
        {
            return false;
        }
        const auto [ValueType, Value] = Current;
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
    Callback(Current.Lexeme, *this);
    Advance();
    return true;
}

bool FHtmlParser::EmptyContent()
{
    Callback({}, *this);
    CloseElement();
    AdvanceMatching(FHtmlScanner::ETokenType::AngleClose);
    return true;
}
