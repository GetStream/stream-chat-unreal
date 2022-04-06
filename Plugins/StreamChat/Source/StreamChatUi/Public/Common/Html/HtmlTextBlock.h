// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/RichTextBlock.h"
#include "CoreMinimal.h"
#include "Framework/Text/IRichTextMarkupParser.h"
#include "HtmlElementStyle.h"
#include "Styling/SlateStyle.h"

#include "HtmlTextBlock.generated.h"

class FHtmlSlateStyleSet final : public FSlateStyleSet
{
public:
    explicit FHtmlSlateStyleSet(const FName& InStyleSetName, const FHtmlElementStyles& InStyles) : FSlateStyleSet{InStyleSetName}, Styles{InStyles}
    {
    }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 27
    virtual const FSlateWidgetStyle* GetWidgetStyleInternal(const FName DesiredTypeName, const FName StyleName) const override;
#else
    virtual const FSlateWidgetStyle* GetWidgetStyleInternal(
        const FName DesiredTypeName,
        const FName StyleName,
        const FSlateWidgetStyle* DefaultStyle,
        bool bWarnIfNotFound) const override;
#endif

private:
    FHtmlElementStyles Styles;
    mutable TMap<FName, FTextBlockStyle> CombinedStyleCache;
};

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UHtmlTextBlock final : public URichTextBlock
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Widget")
    void SetHtmlStyles(const FHtmlElementStyles& InStyles);

protected:
    UPROPERTY(EditAnywhere, Category = Appearance)
    FHtmlElementStyles Styles;

private:
    virtual void CreateDecorators(TArray<TSharedRef<ITextDecorator>>& OutDecorators) override;
    virtual TSharedPtr<IRichTextMarkupParser> CreateMarkupParser() override;
    virtual void UpdateStyleData() override;
};
