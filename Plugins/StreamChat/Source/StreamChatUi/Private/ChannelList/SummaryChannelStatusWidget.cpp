// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/SummaryChannelStatusWidget.h"

#include "ThemeDataAsset.h"
#include "UiBlueprintLibrary.h"
#include "WidgetUtil.h"

bool USummaryChannelStatusWidget::IsForChannel(const UChatChannel* QueryChannel) const
{
    return QueryChannel && StatusChannel && QueryChannel->Properties.Cid == StatusChannel->Properties.Cid;
}

void USummaryChannelStatusWidget::OnSetup()
{
    if (ChannelContextProvider)
    {
        ChannelContextProvider->Setup(StatusChannel);
    }

    if (StatusChannel)
    {
        OnMessagesUpdated();
        OnUnreadChanged(StatusChannel->State.UnreadCount());
    }

    if (StatusChannel && Avatar)
    {
        const TOptional<FString> Image = StatusChannel->Properties.GetImageUrl();
        if (Image.IsSet())
        {
            Avatar->SetupWithUrl(Image.GetValue());
        }
        else
        {
            Avatar->Setup(StatusChannel->Properties.GetOtherMemberUsers());
        }
    }

    UpdateChannelTitleText();

    // Force update channel title
    ChannelTitleAvailableSpace = -1.f;
}

void USummaryChannelStatusWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (RecentMessageTextBlock)
    {
        RecentMessageTextBlock->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->ChannelStatusRecentMessageTextColor));
    }
}

void USummaryChannelStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (StatusChannel)
    {
        StatusChannel->MessagesUpdated.AddDynamic(this, &USummaryChannelStatusWidget::OnMessagesUpdated);
        StatusChannel->UnreadChanged.AddDynamic(this, &USummaryChannelStatusWidget::OnUnreadChanged);
    }
}

void USummaryChannelStatusWidget::NativeDestruct()
{
    if (StatusChannel)
    {
        StatusChannel->MessagesUpdated.RemoveDynamic(this, &USummaryChannelStatusWidget::OnMessagesUpdated);
        StatusChannel->UnreadChanged.RemoveDynamic(this, &USummaryChannelStatusWidget::OnUnreadChanged);
    }
    Super::NativeDestruct();
}

FLinearColor USummaryChannelStatusWidget::GetTitleColor()
{
    return GetTheme()->GetPaletteColor(
        StatusChannel && StatusChannel->Properties.bMuted ? GetTheme()->ChannelStatusMutedTitleTextColor : GetTheme()->ChannelStatusTitleTextColor);
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
    if (TitleTextBlock && StatusChannel)
    {
        const FString Title = UUiBlueprintLibrary::GetChannelTitle(StatusChannel);
        const FString Shortened = WidgetUtil::TruncateWithEllipsis(Title, ChannelTitleAvailableSpace, TitleTextBlock->GetFont());
        const FText Text = FText::FromString(Shortened);
        TitleTextBlock->SetText(Text);
    }
}

void USummaryChannelStatusWidget::UpdateRecentMessageText() const
{
    if (RecentMessageTextBlock && StatusChannel)
    {
        if (!StatusChannel->State.Messages.IsEmpty())
        {
            RecentMessageTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            const FString LastText = StatusChannel->State.Messages.Last().Text;
            const FString Shortened = WidgetUtil::TruncateWithEllipsis(LastText, RecentMessageAvailableSpace, RecentMessageTextBlock->GetFont());
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
    if (Timestamp && StatusChannel)
    {
        if (StatusChannel->State.Messages.IsEmpty())
        {
            Timestamp->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            Timestamp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            Timestamp->Setup(StatusChannel->State.Messages.Last(), false, true);
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
