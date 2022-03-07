// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Header/HeaderWidget.h"

#include "Components/NamedSlot.h"
#include "ThemeDataAsset.h"

UHeaderWidget::UHeaderWidget()
{
    bWantsTheme = true;
    bAutoSetup = true;
}

void UHeaderWidget::SetTitle(const FText& InTitle)
{
    Title = InTitle;
    HeaderTitleTextBlock->SetText(Title);
}

void UHeaderWidget::SetSubtitleContent(UWidget* Content) const
{
    if (!SubtitleSlot)
    {
        return;
    }

    SubtitleSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    SubtitleSlot->SetContent(Content);
}

void UHeaderWidget::SetRightContent(UWidget* Content) const
{
    if (!RightSlot)
    {
        return;
    }

    RightSlot->SetContent(Content);
}

void UHeaderWidget::OnSetup()
{
    if (SubtitleSlot && !SubtitleSlot->HasAnyChildren())
    {
        SubtitleSlot->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (HeaderTitleTextBlock)
    {
        HeaderTitleTextBlock->SetText(Title);
    }
}

void UHeaderWidget::OnTheme()
{
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->HeaderBackgroundColor));
    }
    if (HeaderTitleTextBlock)
    {
        HeaderTitleTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->HeaderTitleTextColor));
    }
}
