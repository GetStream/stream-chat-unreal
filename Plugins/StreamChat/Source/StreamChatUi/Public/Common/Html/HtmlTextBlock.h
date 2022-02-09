// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/RichTextBlock.h"
#include "CoreMinimal.h"
#include "Framework/Text/IRichTextMarkupParser.h"
#include "Styling/SlateStyle.h"

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

    static FString MakeRunName(TSet<FStringView>& ElementNames);
};

#endif    // WITH_FANCY_TEXT

class FComboSlateStyleSet final : public FSlateStyleSet
{
public:
    explicit FComboSlateStyleSet(const FName& InStyleSetName) : FSlateStyleSet(InStyleSetName)
    {
    }

    virtual const FSlateWidgetStyle* GetWidgetStyleInternal(const FName DesiredTypeName, const FName StyleName) const override;

private:
    const TSharedRef<FSlateWidgetStyle>* GetComboStyle(const FName& StyleName);
};

/**
 *
 */
UCLASS() class STREAMCHATUI_API UHtmlTextBlock final : public URichTextBlock
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Widget")
    void SetTextStyles(const TMap<FName, FTextBlockStyle>& InTextStyles);

protected:
    UPROPERTY(EditAnywhere, Category = Appearance)
    TMap<FName, FTextBlockStyle> TextStyles;

private:
    virtual TSharedPtr<IRichTextMarkupParser> CreateMarkupParser() override;
    virtual void UpdateStyleData() override;
};
