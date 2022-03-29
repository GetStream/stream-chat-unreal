// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlDecorators.h"

#include "Algo/Transform.h"
#include "Framework/Text/SlateHyperlinkRun.h"
#include "Framework/Text/SlateImageRun.h"
#include "Framework/Text/SlateTextRun.h"
#include "HtmlParser.h"
#include "Styling/ISlateStyle.h"

const TArray<FName>& FHtmlDecorator::GetTags(const FString& RunName) const
{
    if (TArray<FName>* Found = ParsedTagsCache.Find(RunName))
    {
        return *Found;
    }
    TArray<FString> Tags;
    RunName.ParseIntoArray(Tags, TEXT("_"));
    TArray<FName> TagNames;
    Algo::Transform(Tags, TagNames, [](const FString& Str) { return FName(Str); });
    return ParsedTagsCache.Add(RunName, TagNames);
}

bool FHtmlDecorator::Supports(const FTextRunParseResults& RunInfo, const FString& Text) const
{
    return GetTags(RunInfo.Name).Contains(GetSupportedTag());
}

TSharedRef<ISlateRun> FHtmlDecorator::Create(
    const TSharedRef<FTextLayout>& TextLayout,
    const FTextRunParseResults& ParseResults,
    const FString& ProcessedText,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    check(Style->HasWidgetStyle<FTextBlockStyle>(FName{ParseResults.Name}));

    FRunInfo RunInfo(ParseResults.Name);
    for (const TPair<FString, FTextRange>& Pair : ParseResults.MetaData)
    {
        const int32 Length = FMath::Max(0, Pair.Value.EndIndex - Pair.Value.BeginIndex);
        RunInfo.MetaData.Add(Pair.Key, ProcessedText.Mid(Pair.Value.BeginIndex, Length));
    }

    FTextRange ModelRange;
    ModelRange.BeginIndex = InOutModelText->Len();
    *InOutModelText += ProcessedText.Mid(ParseResults.ContentRange.BeginIndex, ParseResults.ContentRange.EndIndex - ParseResults.ContentRange.BeginIndex);
    ModelRange.EndIndex = InOutModelText->Len();

    return Create(ParseResults, RunInfo, ModelRange, InOutModelText, Style);
}

FName FHyperlinkHtmlDecorator::GetSupportedTag() const
{
    return HtmlTag::Anchor;
}

TSharedRef<ISlateRun> FHyperlinkHtmlDecorator::Create(
    const FTextRunParseResults& ParseResults,
    const FRunInfo& RunInfo,
    const FTextRange& ModelRange,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    FHyperlinkStyle HyperlinkStyle;
    HyperlinkStyle.SetTextStyle(Style->GetWidgetStyle<FTextBlockStyle>(FName{RunInfo.Name}));
    FButtonStyle ButtonStyle;
    ButtonStyle.Normal.DrawAs = ESlateBrushDrawType::NoDrawType;
    ButtonStyle.Hovered.DrawAs = ESlateBrushDrawType::NoDrawType;
    ButtonStyle.Pressed.DrawAs = ESlateBrushDrawType::NoDrawType;
    ButtonStyle.Disabled.DrawAs = ESlateBrushDrawType::NoDrawType;
    HyperlinkStyle.SetUnderlineStyle(ButtonStyle);
    return FSlateHyperlinkRun::Create(
        RunInfo,
        InOutModelText,
        HyperlinkStyle,
        FSlateHyperlinkRun::FOnClick::CreateLambda(
            [](const FSlateHyperlinkRun::FMetadata& Metadata)
            {
                if (const FString* Url = Metadata.Find(TEXT("href")))
                {
                    FPlatformProcess::LaunchURL(**Url, nullptr, nullptr);
                }
            }),
        FSlateHyperlinkRun::FOnGenerateTooltip(),
        FSlateHyperlinkRun::FOnGetTooltipText(),
        ModelRange);
}

FName FListItemHtmlDecorator::GetSupportedTag() const
{
    return HtmlTag::ListItem;
}

TSharedRef<ISlateRun> FListItemHtmlDecorator::Create(
    const FTextRunParseResults& ParseResults,
    const FRunInfo& RunInfo,
    const FTextRange& ModelRange,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    if (GetTags(RunInfo.Name).Contains(HtmlTag::OrderedList))
    {
        if (ParseResults.ContentRange.BeginIndex > LastChar)
        {
            LastChar = ParseResults.ContentRange.BeginIndex;
            ++Order;
        }
        else
        {
            LastChar = INDEX_NONE;
            Order = 0;
        }
        InOutModelText->InsertAt(0, FString::Printf(TEXT("%2d. "), Order));
    }
    else
    {
        // This will do for now!
        InOutModelText->InsertAt(0, TEXT(" • "));
    }
    const FTextBlockStyle& TextBlockStyle = Style->GetWidgetStyle<FTextBlockStyle>(FName{RunInfo.Name});
    return FSlateTextRun::Create(RunInfo, InOutModelText, TextBlockStyle);
}

FName FHorizontalRuleHtmlDecorator::GetSupportedTag() const
{
    return HtmlTag::HorizontalRule;
}

TSharedRef<ISlateRun> FHorizontalRuleHtmlDecorator::Create(
    const FTextRunParseResults& ParseResults,
    const FRunInfo& RunInfo,
    const FTextRange& ModelRange,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    FTextRange ImgRange;
    ImgRange.BeginIndex = InOutModelText->Len();
    *InOutModelText += TEXT('\x200B');    // Zero-Width Breaking Space
    ImgRange.EndIndex = InOutModelText->Len();

    const FInlineTextImageStyle& ImageStyle = Style->GetWidgetStyle<FInlineTextImageStyle>(FName{RunInfo.Name});
    return FSlateImageRun::Create(RunInfo, InOutModelText, &ImageStyle.Image, ImageStyle.Baseline, ImgRange);
}
