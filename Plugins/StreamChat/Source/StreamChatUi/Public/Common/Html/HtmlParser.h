// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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

    FStringView Source;
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
    };

    using FCallbackFn = TFunctionRef<void(const FStringView& Content, const FHtmlParser& Parser)>;

    // Initialize with source string. Doesn't take ownership of string, so caller must ensure it stays in memory.
    // Callback is called on each content chunk as it is found, along with the stack of surrounding element names
    explicit FHtmlParser(const FString& Source, FCallbackFn InCallback);

    // Parse the source string. Returns success.
    bool Parse();

    static constexpr TCHAR ParagraphTag[] = TEXT("p");
    static constexpr TCHAR LineBreakTag[] = TEXT("br");

    int32 Line = 0;
    int32 ParagraphStartIndex = 0;
    int32 ParagraphEndIndex = 0;
    TArray<FElement> ElementStack;

private:
    void Advance();
    bool AdvanceMatching(FHtmlScanner::ETokenType TokenType);

    bool Element();
    bool Attribute();
    bool Content();
    bool EmptyContent();
    void CloseElement();
    void Newline();
    FCallbackFn Callback;
    FHtmlScanner::FToken Current;
    FHtmlScanner Scanner;
};
