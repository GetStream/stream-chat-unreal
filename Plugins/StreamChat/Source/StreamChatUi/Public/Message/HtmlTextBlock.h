// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/RichTextBlock.h"
#include "CoreMinimal.h"
#include "FastXml.h"
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

class FHtmlParser final : public IFastXmlCallback
{
public:
    explicit FHtmlParser(const FString& InInput);
    virtual ~FHtmlParser() = default;
    void Parse();

    FString Output;
    TArray<FTextLineParseResults> LineParseResults;

private:
    virtual bool ProcessXmlDeclaration(const TCHAR* ElementData, int32 XmlFileLineNumber) override;
    virtual bool ProcessElement(const TCHAR* ElementName, const TCHAR* ElementData, int32 XmlFileLineNumber) override;
    virtual bool ProcessAttribute(const TCHAR* AttributeName, const TCHAR* AttributeValue) override;
    virtual bool ProcessClose(const TCHAR* Element) override;
    virtual bool ProcessComment(const TCHAR* Comment) override;

    int32 GetStartIndex(const TCHAR* Str) const;
    int32 GetEndIndex(const TCHAR* Str) const;
    FTextRunParseResults& LastRun();
    FTextLineParseResults& LastResult();
    TArray<FString> ElementStack;
    FString Input;
    TCHAR* InputPtr;
};

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
