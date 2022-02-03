// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/RichTextBlock.h"
#include "CoreMinimal.h"
#include "Framework/Text/IRichTextMarkupParser.h"
#include "Framework/Text/IRichTextMarkupWriter.h"

#include "HtmlTextBlock.generated.h"

#if WITH_FANCY_TEXT

struct FTextLineParseResults;

class STREAMCHATUI_API FHtmlRichTextMarkupParser final : public IRichTextMarkupParser
{
public:
    static TSharedRef<FHtmlRichTextMarkupParser> GetStaticInstance();

    /**
     * Processes the provided Input string producing a set of FTextLineParseResults and a output string stripped of any markup.
     */
    virtual void Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output) override;

private:
    FHtmlRichTextMarkupParser();
};

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
    int32 Current = 0;
    int32 Start = 0;
    bool bInElement = false;
};

class FHtmlParser
{
public:
    struct FElement
    {
        FStringView Name;
        TMap<FStringView, FStringView> Attributes;
    };

    // Initialize with source string. Doesn't take ownership of string, so caller must ensure it stays in memory.
    // Callback is called on each content chunk as it is found, along with the stack of surrounding element names
    explicit FHtmlParser(const FString& Source, TFunctionRef<void(const FStringView& Content, const TArray<FElement>& ElementStack)> InCallback);

    // Parse the source string. Returns success.
    bool Parse();

private:
    void Advance();
    bool AdvanceMatching(FHtmlScanner::ETokenType TokenType);

    bool Element();
    bool Attribute();
    bool Content();
    TFunctionRef<void(const FStringView& Content, const TArray<FElement>& ElementStack)> Callback;
    FHtmlScanner Scanner;
    FHtmlScanner::FToken Current;
    TArray<FElement> ElementStack;
};

class STREAMCHATUI_API FHtmlRichTextMarkupWriter final : public IRichTextMarkupWriter
{
public:
    static TSharedRef<FHtmlRichTextMarkupWriter> GetStaticInstance();

    /**
     * Write the provided array of line and run info, producing an output string containing the markup needed to persist the run layouts
     */
    virtual void Write(const TArray<FRichTextLine>& InLines, FString& Output) override;

private:
    FHtmlRichTextMarkupWriter();
};

#endif    // WITH_FANCY_TEXT

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UHtmlTextBlock final : public URichTextBlock
{
    GENERATED_BODY()

    virtual TSharedPtr<IRichTextMarkupParser> CreateMarkupParser() override;
    virtual TSharedPtr<IRichTextMarkupWriter> CreateMarkupWriter() override;
};
