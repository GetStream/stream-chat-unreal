// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlRichTextBlockDecorator.h"

#include "Components/RichTextBlock.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/SlateWidgetRun.h"

FHtmlRichTextDecorator::FHtmlRichTextDecorator(URichTextBlock* InOwner) : Owner(InOwner)
{
}

FHtmlRichTextDecorator::~FHtmlRichTextDecorator()
{
}

TSharedRef<ISlateRun> FHtmlRichTextDecorator::Create(
    const TSharedRef<FTextLayout>& TextLayout,
    const FTextRunParseResults& RunParseResult,
    const FString& OriginalText,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    FTextRange ModelRange;
    ModelRange.BeginIndex = InOutModelText->Len();

    FTextRunInfo RunInfo(
        RunParseResult.Name,
        FText::FromString(
            OriginalText.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex)));

    for (const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
    {
        RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
    }

    const FTextBlockStyle& TextStyle = Owner->GetCurrentDefaultTextStyle();

    TSharedPtr<ISlateRun> SlateRun;
    if (const TSharedPtr<SWidget> DecoratorWidget = CreateDecoratorWidget(RunInfo, TextStyle); DecoratorWidget.IsValid())
    {
        *InOutModelText += TEXT('\u200B');    // Zero-Width Breaking Space
        ModelRange.EndIndex = InOutModelText->Len();

        // Calculate the baseline of the text within the owning rich text
        const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
        const int16 WidgetBaseline = FontMeasure->GetBaseline(TextStyle.Font) - FMath::Min(0.0f, TextStyle.ShadowOffset.Y);

        const FSlateWidgetRun::FWidgetRunInfo WidgetRunInfo(DecoratorWidget.ToSharedRef(), WidgetBaseline);
        SlateRun = FSlateWidgetRun::Create(TextLayout, RunInfo, InOutModelText, WidgetRunInfo, ModelRange);
    }
    else
    {
        // Assume there's a text handler if widget is empty, if there isn't one it will just display an empty string
        FTextBlockStyle TempStyle = TextStyle;
        CreateDecoratorText(RunInfo, TempStyle, *InOutModelText);

        ModelRange.EndIndex = InOutModelText->Len();
        SlateRun = FSlateTextRun::Create(RunInfo, InOutModelText, TempStyle, ModelRange);
    }

    return SlateRun.ToSharedRef();
}

TSharedPtr<SWidget> FHtmlRichTextDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& DefaultTextStyle) const
{
    return TSharedPtr<SWidget>();
}

void FHtmlRichTextDecorator::CreateDecoratorText(const FTextRunInfo& RunInfo, FTextBlockStyle& InOutTextStyle, FString& InOutString) const
{
}

bool FHtmlRichTextDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
    return RunParseResult.Name == TEXT("em") || RunParseResult.Name == TEXT("strong");
}

TSharedPtr<ITextDecorator> UHtmlRichTextBlockDecorator::CreateDecorator(URichTextBlock* InOwner)
{
    return MakeShared<FHtmlRichTextDecorator>(InOwner);
}
