// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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
    Channel = InChannel;

    Super::Setup();
}

void UChannelStatusWidget::UpdateSelection(UChatChannel* SelectedChannel) const
{
    if (!Button)
    {
        return;
    }

    if (IsForChannel(SelectedChannel))
    {
        Button->SetStyle(SelectedStyle);
    }
    else
    {
        Button->SetStyle(NormalStyle);
    }
}

bool UChannelStatusWidget::IsForChannel(const UChatChannel* QueryChannel) const
{
    return QueryChannel && Channel && QueryChannel->Properties.Cid == Channel->Properties.Cid;
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
        Channel->UnreadChanged.AddDynamic(this, &UChannelStatusWidget::OnUnreadChanged);
        OnMessagesUpdated(Channel->GetMessages());
        OnUnreadChanged(Channel->State.UnreadCount());
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
}

void UChannelStatusWidget::OnTheme()
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
        const FName Color{Channel && Channel->Properties.bMuted ? Theme->ChannelStatusMutedTitleTextColor : Theme->ChannelStatusTitleTextColor};
        TitleTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Color));
    }

    if (RecentMessageTextBlock)
    {
        RecentMessageTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->ChannelStatusRecentMessageTextColor));
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

void UChannelStatusWidget::OnMessagesUpdated(const TArray<FMessage>& Messages)
{
    if (Timestamp)
    {
        if (Messages.Num() > 0)
        {
            Timestamp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            Timestamp->Setup(Messages.Last(), false, true);
        }
        else
        {
            Timestamp->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // Force update recent message text
    RecentMessageAvailableSpace = -1.f;
}

void UChannelStatusWidget::OnUnreadChanged(const int32 UnreadCount)
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

void UChannelStatusWidget::OnButtonClicked()
{
    OnChannelStatusButtonClickedNative.Broadcast(Channel);
    OnChannelStatusButtonClicked.Broadcast(Channel);
}
