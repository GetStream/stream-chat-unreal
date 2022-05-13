// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/SummaryChannelStatusWidget.h"

#include "ThemeDataAsset.h"
#include "UiBlueprintLibrary.h"
#include "WidgetUtil.h"

bool USummaryChannelStatusWidget::IsForChannel(const UChatChannel* QueryChannel) const
{
    return QueryChannel && Channel && QueryChannel->Properties.Cid == Channel->Properties.Cid;
}

void USummaryChannelStatusWidget::OnSetup()
{
    Super::OnSetup();

    if (ChannelContextProvider)
    {
        ChannelContextProvider->Setup(Channel);
    }

    if (Channel)
    {
        Channel->MessagesUpdated.AddDynamic(this, &USummaryChannelStatusWidget::OnMessagesUpdated);
        Channel->UnreadChanged.AddDynamic(this, &USummaryChannelStatusWidget::OnUnreadChanged);
        OnMessagesUpdated();
        OnUnreadChanged(Channel->State.UnreadCount());
    }

    if (Channel && Avatar)
    {
        Avatar->Setup(Channel->Properties.GetOtherMemberUsers());
    }

    // Force update channel title
    ChannelTitleAvailableSpace = -1.f;
}

void USummaryChannelStatusWidget::OnTheme()
{
    Super::OnTheme();

    if (RecentMessageTextBlock)
    {
        RecentMessageTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->ChannelStatusRecentMessageTextColor));
    }
}

FLinearColor USummaryChannelStatusWidget::GetTitleColor()
{
    return Theme->GetPaletteColor(Channel && Channel->Properties.bMuted ? Theme->ChannelStatusMutedTitleTextColor : Theme->ChannelStatusTitleTextColor);
}

int32 USummaryChannelStatusWidget::NativePaint(
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
        const float AvailableSpace = RecentMessageTextBlock->GetTickSpaceGeometry().GetLocalSize().X;
        if (AvailableSpace != RecentMessageAvailableSpace)
        {
            RecentMessageAvailableSpace = AvailableSpace;
            UpdateRecentMessageText();
        }
    }

    if (TitleTextBlock)
    {
        const float AvailableSpace = TitleTextBlock->GetTickSpaceGeometry().GetLocalSize().X;
        if (AvailableSpace != ChannelTitleAvailableSpace)
        {
            ChannelTitleAvailableSpace = AvailableSpace;
            UpdateChannelTitleText();
        }
    }

    return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void USummaryChannelStatusWidget::UpdateChannelTitleText() const
{
    if (TitleTextBlock && Channel)
    {
        const FString Title = UUiBlueprintLibrary::GetChannelTitle(Channel);
        const FString Shortened = WidgetUtil::TruncateWithEllipsis(Title, ChannelTitleAvailableSpace, TitleTextBlock->Font);
        const FText Text = FText::FromString(Shortened);
        TitleTextBlock->SetText(Text);
    }
}

void USummaryChannelStatusWidget::UpdateRecentMessageText() const
{
    if (RecentMessageTextBlock && Channel)
    {
        if (!Channel->State.Messages.IsEmpty())
        {
            RecentMessageTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            const FString LastText = Channel->State.Messages.Last().Text;
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

void USummaryChannelStatusWidget::OnMessagesUpdated()
{
    if (Timestamp && Channel)
    {
        if (Channel->State.Messages.IsEmpty())
        {
            Timestamp->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            Timestamp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            Timestamp->Setup(Channel->State.Messages.Last(), false, true);
        }
    }

    // Force update recent message text
    RecentMessageAvailableSpace = -1.f;
}

void USummaryChannelStatusWidget::OnUnreadChanged(const int32 UnreadCount)
{
    if (Notification)
    {
        const ESlateVisibility Vis = UnreadCount > 0 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
        Notification->SetVisibility(Vis);
    }
    if (NotificationTextBlock)
    {
        const FText Text = UnreadCount > 9 ? FText::FromString(TEXT("9+")) : FText::AsNumber(UnreadCount);
        NotificationTextBlock->SetText(Text);
    }
}
