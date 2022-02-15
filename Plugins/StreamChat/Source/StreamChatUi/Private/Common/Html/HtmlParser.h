// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace HtmlTag
{
const FName Anchor{TEXT("a")};
const FName Strong{TEXT("strong")};
const FName Emphasis{TEXT("em")};
const FName InlineCode{TEXT("code")};
const FName DeletedText{TEXT("del")};
const FName Heading1{TEXT("h1")};
const FName Heading2{TEXT("h2")};
const FName Heading3{TEXT("h3")};
const FName Heading4{TEXT("h4")};
const FName Heading5{TEXT("h5")};
const FName Heading6{TEXT("h6")};
const FName UnorderedList{TEXT("ul")};
const FName OrderedList{TEXT("ol")};
const FName ListItem{TEXT("li")};
const FName Paragraph{TEXT("p")};
const FName LineBreak{TEXT("br")};
}    // namespace HtmlTag

class FHtmlScanner
{
public:
    explicit FHtmlScanner(const FString& InSource);

    enum class ETokenType : uint8
    {
        AngleOpen,
        AngleClose,
        Slash,
        Equal,
        String,
        Identifier,
        Content,
        Eof,
        Error
    };

    struct FToken
    {
        ETokenType Type;
        FStringView Lexeme;
    };

    FToken ScanToken();
    const FString& GetOutput() const;
    int32 PrevStart = 0;
    int32 Start = 0;
    int32 Current = 0;

private:
    TCHAR Advance();
    void SkipWhitespace();

    bool IsAtEnd() const;
    TCHAR Peek() const;

    FToken MakeToken(ETokenType Type) const;

    FToken String();
    FToken Identifier();
    FToken Content();

    FString Source;
    bool bInTag = false;
};

/**
 * @brief Parses a subset of XHTML
 * Does NOT support:
 * - empty attribute syntax
 * - unquoted attribute value syntax
 * - single-quoted attribute value syntax
 * - void elements without the trailing slash
 */
class FHtmlParser
{
public:
    struct FElement
    {
        FStringView Name;
        TMap<FStringView, FStringView> Attributes;
        int32 OpeningTagStart;
    };

    using FCallbackFn = TFunctionRef<void(const FHtmlParser& Parser)>;

    // Initialize with source string. Doesn't take ownership of string, so caller must ensure it stays in memory.
    // Callback is called on each content chunk as it is found, along with the stack of surrounding element names
    explicit FHtmlParser(const FString& Source, FCallbackFn InCallback);

    // Parse the source string. Returns success.
    bool Parse();

    // The range of the content in the current run
    // <strong>Hello<em> world</em</strong>
    //         ^^^^^
    FTextRange GetContentRange() const;
    // The range from the start of the current element, to the end of the current content
    // <strong>Hello<em> world</em</strong>
    // ^^^^^^^^^^^^^
    FTextRange GetOriginalRange() const;
    FStringView GetContent() const;
    const FString& GetOutput() const;

    static constexpr TCHAR ParagraphTag[] = TEXT("p");
    static constexpr TCHAR LineBreakTag[] = TEXT("br");
    static constexpr TCHAR ListItemTag[] = TEXT("li");

    int32 Line = 0;
    int32 ParagraphStartIndex = 0;
    TArray<FElement> ElementStack;

private:
    void Advance();
    bool AdvanceMatching(FHtmlScanner::ETokenType TokenType);

    bool Element();
    bool Attribute();
    bool Content();
    void CloseElement();
    void Newline(uint32 Index);
    FCallbackFn Callback;
    FHtmlScanner::FToken Current;
    FHtmlScanner Scanner;
};
