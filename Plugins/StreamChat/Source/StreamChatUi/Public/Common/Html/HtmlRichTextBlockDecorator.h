// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/RichTextBlockDecorator.h"
#include "CoreMinimal.h"

#include "HtmlRichTextBlockDecorator.generated.h"

class STREAMCHATUI_API FHtmlRichTextDecorator final : public ITextDecorator
{
public:
    explicit FHtmlRichTextDecorator(URichTextBlock* InOwner);

    virtual ~FHtmlRichTextDecorator() override;

    /** Override this function to specify which types of tags are handled by this decorator */
    virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;

    virtual TSharedRef<ISlateRun> Create(
        const TSharedRef<class FTextLayout>& TextLayout,
        const FTextRunParseResults& RunParseResult,
        const FString& OriginalText,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) override final;

protected:
    /** Override this function if you want to create a unique widget like an image */
    virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& DefaultTextStyle) const;

    /** Override this function if you want to dynamically generate text, optionally changing the style. InOutString will start as the content between tags */
    virtual void CreateDecoratorText(const FTextRunInfo& RunInfo, FTextBlockStyle& InOutTextStyle, FString& InOutString) const;

    URichTextBlock* Owner;
};

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UHtmlRichTextBlockDecorator final : public URichTextBlockDecorator
{
    GENERATED_BODY()

    virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
};
