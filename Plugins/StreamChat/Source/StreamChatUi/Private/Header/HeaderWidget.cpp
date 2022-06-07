// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Header/HeaderWidget.h"

#include "Components/NamedSlot.h"
#include "ThemeDataAsset.h"

UHeaderWidget::UHeaderWidget()
{
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

void UHeaderWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (Border)
    {
        Border->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->HeaderBackgroundColor));
    }
    if (Button)
    {
        ButtonStyle.Normal.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonNormalBackgroundColor)};
        ButtonStyle.Pressed.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonSelectedBackgroundColor)};
        ButtonStyle.Hovered.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonHoveredBackgroundColor)};
        Button->SetStyle(ButtonStyle);
    }
    if (HeaderTitleTextBlock)
    {
        HeaderTitleTextBlock->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->HeaderTitleTextColor));
    }
}

void UHeaderWidget::OnButtonClick()
{
    OnHeaderButtonClicked.Broadcast();
}
