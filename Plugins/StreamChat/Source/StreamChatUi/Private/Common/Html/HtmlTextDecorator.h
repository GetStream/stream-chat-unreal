// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Text/ITextDecorator.h"

class FHtmlTextDecorator : public ITextDecorator
{
public:
    virtual bool Supports(const FTextRunParseResults& RunInfo, const FString& Text) const override;
    virtual TSharedRef<ISlateRun> Create(
        const TSharedRef<FTextLayout>& TextLayout,
        const FTextRunParseResults& TextRun,
        const FString& ProcessedText,
        const TSharedRef<FString>& InOutModelText,
        const ISlateStyle* Style) override;
};
