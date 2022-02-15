// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Text/ITextDecorator.h"

class FHtmlDecorator : public ITextDecorator
{
public:
    virtual bool Supports(const FTextRunParseResults& RunInfo, const FString& Text) const override;
    virtual TSharedRef<ISlateRun> Create(
        const TSharedRef<FTextLayout>& TextLayout,
        const FTextRunParseResults& TextRun,
        const FString& ProcessedText,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) override;

protected:
    virtual FString GetSupportedTag() const = 0;
    virtual TSharedRef<ISlateRun> Create(
        const FName& Tags,
        const FRunInfo& RunInfo,
        const FTextRange& ModelRange,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) = 0;
};

class FHyperlinkHtmlDecorator final : public FHtmlDecorator
{
protected:
    virtual FString GetSupportedTag() const override;
    virtual TSharedRef<ISlateRun> Create(
        const FName& Tags,
        const FRunInfo& RunInfo,
        const FTextRange& ModelRange,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) override;
};

class FListItemHtmlDecorator final : public FHtmlDecorator
{
protected:
    virtual FString GetSupportedTag() const override;
    virtual TSharedRef<ISlateRun> Create(
        const FName& Tags,
        const FRunInfo& RunInfo,
        const FTextRange& ModelRange,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) override;
};
