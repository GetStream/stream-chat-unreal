#include "Common/Html/HtmlDecorators.h"

#include "Framework/Text/SlateHyperlinkRun.h"
#include "Framework/Text/SlateTextRun.h"
#include "HtmlParser.h"
#include "Styling/ISlateStyle.h"

bool FHtmlDecorator::Supports(const FTextRunParseResults& RunInfo, const FString& Text) const
{
    TArray<FString> Tags;
    RunInfo.Name.ParseIntoArray(Tags, TEXT("_"));
    return Tags.Contains(GetSupportedTag());
}

TSharedRef<ISlateRun> FHtmlDecorator::Create(
    const TSharedRef<FTextLayout>& TextLayout,
    const FTextRunParseResults& TextRun,
    const FString& ProcessedText,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    const FName Tags{TextRun.Name};
    check(Style->HasWidgetStyle<FTextBlockStyle>(Tags));

    FRunInfo RunInfo(TextRun.Name);
    for (const TPair<FString, FTextRange>& Pair : TextRun.MetaData)
    {
        const int32 Length = FMath::Max(0, Pair.Value.EndIndex - Pair.Value.BeginIndex);
        RunInfo.MetaData.Add(Pair.Key, ProcessedText.Mid(Pair.Value.BeginIndex, Length));
    }

    FTextRange ModelRange;
    ModelRange.BeginIndex = InOutModelText->Len();
    *InOutModelText += ProcessedText.Mid(TextRun.ContentRange.BeginIndex, TextRun.ContentRange.EndIndex - TextRun.ContentRange.BeginIndex);
    ModelRange.EndIndex = InOutModelText->Len();

    return Create(Tags, RunInfo, ModelRange, InOutModelText, Style);
}

FString FHyperlinkHtmlDecorator::GetSupportedTag() const
{
    static FString SupportedTag = HtmlTag::Anchor.ToString();
    return SupportedTag;
}

TSharedRef<ISlateRun> FHyperlinkHtmlDecorator::Create(
    const FName& Tags,
    const FRunInfo& RunInfo,
    const FTextRange& ModelRange,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    FHyperlinkStyle HyperlinkStyle;
    HyperlinkStyle.SetTextStyle(Style->GetWidgetStyle<FTextBlockStyle>(Tags));
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

FString FListItemHtmlDecorator::GetSupportedTag() const
{
    static FString SupportedTag = HtmlTag::ListItem.ToString();
    return SupportedTag;
}

TSharedRef<ISlateRun> FListItemHtmlDecorator::Create(
    const FName& Tags,
    const FRunInfo& RunInfo,
    const FTextRange& ModelRange,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    // This will do for now!
    InOutModelText->InsertAt(0, TEXT(" • "));
    const FTextBlockStyle TextBlockStyle = Style->GetWidgetStyle<FTextBlockStyle>(Tags);
    return FSlateTextRun::Create(RunInfo, InOutModelText, TextBlockStyle);
}
