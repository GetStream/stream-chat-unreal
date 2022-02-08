// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Message/HtmlTextBlock.h"

#include "Algo/Transform.h"
#include "Widgets/Text/SRichTextBlock.h"

namespace
{
constexpr TCHAR ParagraphTag[] = TEXT("p");
constexpr TCHAR LineBreakTag[] = TEXT("br");

template <class ObjectType>
struct TFDeferredDeleter final : FDeferredCleanupInterface
{
    explicit TFDeferredDeleter(ObjectType* InInnerObjectToDelete) : InnerObjectToDelete(InInnerObjectToDelete)
    {
    }

    virtual ~TFDeferredDeleter() override
    {
        delete InnerObjectToDelete;
    }

private:
    ObjectType* InnerObjectToDelete;
};

template <class ObjectType>
FORCEINLINE SharedPointerInternals::FRawPtrProxy<ObjectType> MakeShareableDeferredCleanup(ObjectType* InObject)
{
    return MakeShareable(InObject, [](ObjectType* ObjectToDelete) { BeginCleanup(new TFDeferredDeleter<ObjectType>(ObjectToDelete)); });
}
}    // namespace

TSharedRef<FHtmlRichTextMarkupParser> FHtmlRichTextMarkupParser::GetStaticInstance()
{
    static TSharedRef<FHtmlRichTextMarkupParser> Parser = MakeShareable(new FHtmlRichTextMarkupParser());
    return Parser;
}

void FHtmlRichTextMarkupParser::Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output)
{
    FHtmlParser Parser(
        Input,
        [&](const FStringView& Content, const FHtmlParser& Self)
        {
            if (!Results.IsValidIndex(Self.Line))
            {
                Results.Add(FTextLineParseResults{{Self.ParagraphStartIndex, INDEX_NONE}});
            }
            // Keep overwriting end index with each iteration, eventually leaving the correct result
            Results[Self.Line].Range.EndIndex = Input.Len();

            TSet<FStringView> ElementNames;
            Algo::Transform(Self.ElementStack, ElementNames, [](auto&& Element) { return Element.Name; });
            const FString Name = MakeRunName(ElementNames);

            // ContentRange indexes into the Output string
            const FTextRange ContentRange{Output.Len(), Output.Len() + Content.Len()};
            const FTextRunParseResults Run{Name, ContentRange, ContentRange};

            Results[Self.Line].Runs.Add(Run);

            Output.Append(Content);
        });
    Parser.Parse();
}

FHtmlRichTextMarkupParser::FHtmlRichTextMarkupParser()
{
}

FString FHtmlRichTextMarkupParser::MakeRunName(TSet<FStringView>& ElementNames)
{
    ElementNames.Remove(ParagraphTag);
    ElementNames.Remove(LineBreakTag);
    if (ElementNames.Num() == 0)
    {
        return TEXT("Default");
    }

    ElementNames.Sort([](const FStringView& A, const FStringView& B) { return A.Compare(B) < 0; });
    return FString::Join(ElementNames, TEXT("_"));
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

FHtmlScanner::FToken FHtmlScanner::ScanToken()
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

FHtmlParser::FHtmlParser(const FString& Source, FCallbackFn InCallback) : Scanner(FHtmlScanner(Source)), Callback(MoveTemp(InCallback))
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
    // Any <p> will implicitly close any open <p>, hence increment line number
    // <br> always starts new line
    if ((Current.Lexeme.Equals(ParagraphTag) || Current.Lexeme.Equals(LineBreakTag)) && ElementCount > 0)
    {
        ParagraphEndIndex = ParagraphStartIndex;
        ParagraphStartIndex = Scanner.Start;
        ++Line;
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
            if (!EmptyContent())
            {
                return false;
            }
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
    ++ElementCount;

    ElementStack.Pop();
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
    Callback(Current.Lexeme, *this);
    Advance();
    return true;
}

bool FHtmlParser::EmptyContent()
{
    Callback({}, *this);
    CloseElement();
    return true;
}

const FSlateWidgetStyle* FComboSlateStyleSet::GetWidgetStyleInternal(const FName DesiredTypeName, const FName StyleName) const
{
    const TSharedRef<FSlateWidgetStyle>* StylePtr = WidgetStyleValues.Find(StyleName);

    if (!StylePtr)
    {
        StylePtr = const_cast<FComboSlateStyleSet*>(this)->GetComboStyle(StyleName);    // sorry :(
        if (!StylePtr)
        {
            Log(Warning,
                FText::Format(
                    FText::FromString(TEXT("Unable to find Slate Widget Style '{0}'. Using {1} defaults instead.")),
                    FText::FromName(StyleName),
                    FText::FromName(DesiredTypeName)));
            return nullptr;
        }
    }

    const TSharedRef<FSlateWidgetStyle> Style = *StylePtr;

    if (Style->GetTypeName() != DesiredTypeName)
    {
        Log(Error,
            FText::Format(
                FText::FromString(TEXT("The Slate Widget Style '{0}' is not of the desired type. Desired: '{1}', Actual: '{2}'")),
                FText::FromName(StyleName),
                FText::FromName(DesiredTypeName),
                FText::FromName(Style->GetTypeName())));
        return nullptr;
    }

    return &Style.Get();
}

template <class T, class U>
void OverwriteNonDefault(const U& Default, U& Target, const U& New, T U::*MemPtr)
{
    if (New.*MemPtr != Default.*MemPtr)
    {
        Target.*MemPtr = New.*MemPtr;
    }
}

const TSharedRef<FSlateWidgetStyle>* FComboSlateStyleSet::GetComboStyle(const FName& StyleName)
{
    const TSharedRef<FSlateWidgetStyle>* DefaultPtr = WidgetStyleValues.Find(TEXT("Default"));
    if (!DefaultPtr)
    {
        return nullptr;
    }
    const TSharedRef<FTextBlockStyle> Default = StaticCastSharedRef<FTextBlockStyle>(*DefaultPtr);

    TArray<FString> Tags;
    StyleName.ToString().ParseIntoArray(Tags, TEXT("_"));

    const TSharedRef<FTextBlockStyle> ComboStyle = MakeShared<FTextBlockStyle>(*Default);
    for (const FString& Tag : Tags)
    {
        const TSharedRef<FSlateWidgetStyle>* StylePtr = WidgetStyleValues.Find(FName(Tag));
        if (!StylePtr)
        {
            return nullptr;
        }
        const TSharedRef<FTextBlockStyle> Style = StaticCastSharedRef<FTextBlockStyle>(*StylePtr);
        if (Style->Font.Size != Default->Font.Size)
        {
            ComboStyle->Font.Size = Style->Font.Size;
        }
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::FontObject);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::FontMaterial);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::CompositeFont);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::TypefaceFontName);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::Size);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::LetterSpacing);
        // OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::OutlineSettings);

        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::ColorAndOpacity);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::ShadowOffset);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::ShadowColorAndOpacity);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::SelectedBackgroundColor);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::HighlightColor);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::HighlightShape);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::StrikeBrush);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::UnderlineBrush);
    }
    return &WidgetStyleValues.Add(StyleName, ComboStyle);
}

TSharedPtr<IRichTextMarkupParser> UHtmlTextBlock::CreateMarkupParser()
{
    return FHtmlRichTextMarkupParser::GetStaticInstance();
}

void UHtmlTextBlock::UpdateStyleData()
{
    StyleInstance = MakeShareableDeferredCleanup(new FComboSlateStyleSet(TEXT("HtmlTextStyle")));

    for (const auto& [Name, Style] : TextStyles)
    {
        if (Name == TEXT("Default"))
        {
            DefaultTextStyle = Style;
        }

        StyleInstance->Set(Name, Style);
    }
}
