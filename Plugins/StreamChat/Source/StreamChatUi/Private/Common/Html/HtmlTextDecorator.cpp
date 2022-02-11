#include "Common/Html/HtmlTextDecorator.h"

#include "Framework/Text/SlateHyperlinkRun.h"

bool FHtmlTextDecorator::Supports(const FTextRunParseResults& RunInfo, const FString& Text) const
{
    return RunInfo.Name == TEXT("a");
}

TSharedRef<ISlateRun> FHtmlTextDecorator::Create(
    const TSharedRef<FTextLayout>& TextLayout,
    const FTextRunParseResults& TextRun,
    const FString& OriginalText,
    const TSharedRef<FString>& InOutModelText,
    const ISlateStyle* Style)
{
    const FName Tag{TextRun.Name};
    check(Style->HasWidgetStyle<FTextBlockStyle>(Tag));

    FRunInfo RunInfo(TextRun.Name);
    for (const TPair<FString, FTextRange>& Pair : TextRun.MetaData)
    {
        const int32 Length = FMath::Max(0, Pair.Value.EndIndex - Pair.Value.BeginIndex);
        RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Length));
    }

    FTextRange ModelRange;
    ModelRange.BeginIndex = InOutModelText->Len();
    *InOutModelText += OriginalText.Mid(TextRun.ContentRange.BeginIndex, TextRun.ContentRange.EndIndex - TextRun.ContentRange.BeginIndex);
    ModelRange.EndIndex = InOutModelText->Len();

    check(Tag == TEXT("a"));
    FHyperlinkStyle HyperlinkStyle;
    HyperlinkStyle.SetTextStyle(Style->GetWidgetStyle<FTextBlockStyle>(Tag));
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
