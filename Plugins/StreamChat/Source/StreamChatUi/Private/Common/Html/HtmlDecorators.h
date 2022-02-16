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
        const FTextRunParseResults& ParseResults,
        const FString& ProcessedText,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) override;

protected:
    virtual FName GetSupportedTag() const = 0;
    virtual TSharedRef<ISlateRun> Create(
        const FTextRunParseResults& ParseResults,
        const FRunInfo& RunInfo,
        const FTextRange& ModelRange,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) = 0;

    const TArray<FName>& GetTags(const FString& RunName) const;
    mutable TMap<FString, TArray<FName>> ParsedTagsCache;
};

class FHyperlinkHtmlDecorator final : public FHtmlDecorator
{
protected:
    virtual FName GetSupportedTag() const override;
    virtual TSharedRef<ISlateRun> Create(
        const FTextRunParseResults& ParseResults,
        const FRunInfo& RunInfo,
        const FTextRange& ModelRange,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) override;
};

class FListItemHtmlDecorator final : public FHtmlDecorator
{
protected:
    virtual FName GetSupportedTag() const override;
    virtual TSharedRef<ISlateRun> Create(
        const FTextRunParseResults& ParseResults,
        const FRunInfo& RunInfo,
        const FTextRange& ModelRange,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) override;

    int32 LastChar = INDEX_NONE;
    uint32 Order = 0;
};
