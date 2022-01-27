// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelStatusWidget.h"

#include "Algo/Transform.h"
#include "ThemeDataAsset.h"
#include "UiBlueprintLibrary.h"
#include "WidgetUtil.h"

UChannelStatusWidget::UChannelStatusWidget()
{
    bWantsTheme = true;
}

void UChannelStatusWidget::Setup(UChatChannel* InChannel)
{
    // Channel needs to be provided as this widget won't be under a ChannelContextProvider in the hierarchy
    Channel = InChannel;

    Super::Setup();
}

void UChannelStatusWidget::UpdateSelection(UChatChannel* SelectedChannel) const
{
    if (!Button)
    {
        return;
    }

    if (SelectedChannel->Properties.Cid == Channel->Properties.Cid)
    {
        Button->SetStyle(SelectedStyle);
    }
    else
    {
        Button->SetStyle(NormalStyle);
    }
}

void UChannelStatusWidget::OnSetup()
{
    if (ChannelContextProvider)
    {
        ChannelContextProvider->Setup(Channel);
    }

    if (Channel)
    {
        Channel->MessagesUpdated.AddDynamic(this, &UChannelStatusWidget::OnMessagesUpdated);
    }

    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UChannelStatusWidget::OnButtonClicked);
    }

    if (Channel && Avatar)
    {
        Avatar->Setup(Channel->Properties.GetOtherMemberUsers());
    }

    // Force update channel title
    ChannelTitleAvailableSpace = -1.f;
    UpdateDynamic();
}

void UChannelStatusWidget::OnTheme(const UThemeDataAsset* Theme)
{
    NormalStyle.Normal.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ChannelStatusNormalBackgroundColor)};
    NormalStyle.Pressed.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ChannelStatusSelectedBackgroundColor)};
    NormalStyle.Hovered.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ChannelStatusHoveredBackgroundColor)};
    SelectedStyle.Normal.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ChannelStatusSelectedBackgroundColor)};
    SelectedStyle.Pressed.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ChannelStatusSelectedBackgroundColor)};
    SelectedStyle.Hovered.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ChannelStatusSelectedBackgroundColor)};
    if (Button)
    {
        Button->SetStyle(NormalStyle);
    }

    if (Divider)
    {
        Divider->SetColorAndOpacity(Theme->GetPaletteColor(Theme->ChannelStatusDividerColor));
    }

    if (TitleTextBlock)
    {
        const FName Color{Channel->Properties.bMuted ? Theme->ChannelStatusMutedTitleTextColor : Theme->ChannelStatusTitleTextColor};
        TitleTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Color));
    }
}

int32 UChannelStatusWidget::NativePaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    const int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    const bool bParentEnabled) const
{
    if (RecentMessageTextBlock)
    {
        if (const float AvailableSpace = RecentMessageTextBlock->GetTickSpaceGeometry().GetLocalSize().X; AvailableSpace != RecentMessageAvailableSpace)
        {
            RecentMessageAvailableSpace = AvailableSpace;
            UpdateRecentMessageText();
        }
    }

    if (TitleTextBlock)
    {
        if (const float AvailableSpace = TitleTextBlock->GetTickSpaceGeometry().GetLocalSize().X; AvailableSpace != ChannelTitleAvailableSpace)
        {
            ChannelTitleAvailableSpace = AvailableSpace;
            UpdateChannelTitleText();
        }
    }

    return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void UChannelStatusWidget::UpdateDynamic() const
{
    if (Channel && Timestamp)
    {
        if (Channel->State.GetMessages().Num() > 0)
        {
            Timestamp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            Timestamp->Setup(Channel->State.GetMessages().Last(), false, true);
        }
        else
        {
            Timestamp->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // Force update recent message text
    RecentMessageAvailableSpace = -1.f;
}

void UChannelStatusWidget::UpdateChannelTitleText() const
{
    if (TitleTextBlock && Channel)
    {
        const FString Title = UUiBlueprintLibrary::GetChannelTitle(Channel);
        const FString Shortened = WidgetUtil::TruncateWithEllipsis(Title, ChannelTitleAvailableSpace, TitleTextBlock->Font);
        const FText Text = FText::FromString(Shortened);
        TitleTextBlock->SetText(Text);
    }
}

void UChannelStatusWidget::UpdateRecentMessageText() const
{
    if (RecentMessageTextBlock && Channel)
    {
        if (Channel->State.GetMessages().Num() > 0)
        {
            RecentMessageTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            const FString LastText = Channel->State.GetMessages().Last().Text;
            const FString Shortened = WidgetUtil::TruncateWithEllipsis(LastText, RecentMessageAvailableSpace, RecentMessageTextBlock->Font);
            const FText Text = FText::FromString(Shortened);
            RecentMessageTextBlock->SetText(Text);
        }
        else
        {
            RecentMessageTextBlock->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UChannelStatusWidget::OnMessagesUpdated(const TArray<FMessage>&)
{
    UpdateDynamic();
}

void UChannelStatusWidget::OnButtonClicked()
{
    OnChannelStatusButtonClickedNative.Broadcast(Channel);
    OnChannelStatusButtonClicked.Broadcast(Channel);
}
