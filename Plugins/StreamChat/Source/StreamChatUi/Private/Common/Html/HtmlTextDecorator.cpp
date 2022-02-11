#include "Common/Html/HtmlTextDecorator.h"

#include "Framework/Text/SlateHyperlinkRun.h"
#include "Styling/ISlateStyle.h"

bool FHtmlTextDecorator::Supports(const FTextRunParseResults& RunInfo, const FString& Text) const
{
    TArray<FString> Tags;
    RunInfo.Name.ParseIntoArray(Tags, TEXT("_"));
    return Tags.Contains(TEXT("a"));
}

TSharedRef<ISlateRun> FHtmlTextDecorator::Create(
    const TSharedRef<FTextLayout>& TextLayout,
    const FTextRunParseResults& TextRun,
    const FString& ProcessedText,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    const FName Tag{TextRun.Name};
    check(Style->HasWidgetStyle<FTextBlockStyle>(Tag));

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

    FHyperlinkStyle HyperlinkStyle;
    HyperlinkStyle.SetTextStyle(Style->GetWidgetStyle<FTextBlockStyle>(Tag));
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
