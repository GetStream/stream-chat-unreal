// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/RichTextBlock.h"
#include "CoreMinimal.h"
#include "Framework/Text/IRichTextMarkupParser.h"

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
    FHtmlRichTextMarkupParser() = default;
};

#endif    // WITH_FANCY_TEXT
