// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelStatusWidget.h"

#include "Algo/Transform.h"
#include "UiBlueprintLibrary.h"
#include "WidgetUtil.h"

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

    if (SelectedChannel->State.Cid == Channel->State.Cid)
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
    if (!Channel)
    {
        return;
    }

    Channel->MessagesUpdated.AddDynamic(this, &UChannelStatusWidget::OnMessagesUpdated);

    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UChannelStatusWidget::OnButtonClicked);
        Button->SetStyle(NormalStyle);
    }

    if (Avatar)
    {
        Avatar->Setup(Channel->State.GetOtherMemberUsers());
    }

    // Force update channel title
    ChannelTitleAvailableSpace = -1.f;
    UpdateDynamic();
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
    if (!Channel)
    {
        return;
    }

    if (Timestamp)
    {
        Timestamp->Setup(Channel->State.GetMessages().Last(), false, true);
    }

    // Force update recent message text
    RecentMessageAvailableSpace = -1.f;
}

void UChannelStatusWidget::UpdateChannelTitleText() const
{
    if (TitleTextBlock)
    {
        const FString Title = UUiBlueprintLibrary::GetChannelTitle(Channel);
        const FString Shortened = WidgetUtil::TruncateWithEllipsis(Title, ChannelTitleAvailableSpace, TitleTextBlock->Font);
        const FText Text = FText::FromString(Shortened);
        TitleTextBlock->SetText(Text);
    }
}

void UChannelStatusWidget::UpdateRecentMessageText() const
{
    if (RecentMessageTextBlock)
    {
        const FString Shortened =
            WidgetUtil::TruncateWithEllipsis(Channel->State.GetMessages().Last().Text, RecentMessageAvailableSpace, RecentMessageTextBlock->Font);
        const FText Text = FText::FromString(Shortened);
        RecentMessageTextBlock->SetText(Text);
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
