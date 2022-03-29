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

    if (RightSlot && !RightSlot->HasAnyChildren())
    {
        RightSlot->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (HeaderTitleTextBlock)
    {
        HeaderTitleTextBlock->SetText(Title);
    }

    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UHeaderWidget::OnButtonClick);
    }
}

void UHeaderWidget::OnTheme()
{
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->HeaderBackgroundColor));
    }
    if (Button)
    {
        ButtonStyle.Normal.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonNormalBackgroundColor)};
        ButtonStyle.Pressed.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonSelectedBackgroundColor)};
        ButtonStyle.Hovered.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonHoveredBackgroundColor)};
        Button->SetStyle(ButtonStyle);
    }
    if (HeaderTitleTextBlock)
    {
        HeaderTitleTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->HeaderTitleTextColor));
    }
}

void UHeaderWidget::OnButtonClick()
{
    OnHeaderButtonClicked.Broadcast();
}
