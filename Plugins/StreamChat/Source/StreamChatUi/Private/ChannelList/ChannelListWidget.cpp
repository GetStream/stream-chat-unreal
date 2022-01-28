// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelListWidget.h"

#include "StreamChatClientComponent.h"

UChannelListWidget::UChannelListWidget()
{
    bWantsClient = true;
}

void UChannelListWidget::OnClient()
{
    Client->ChannelsUpdated.AddDynamic(this, &UChannelListWidget::OnChannelsUpdated);
}

void UChannelListWidget::ChannelStatusClicked(UChatChannel* ClickedChannel)
{
    if (ClickedChannel == CurrentChannel)
    {
        return;
    }
    CurrentChannel = ClickedChannel;

    for (UWidget* Child : ChannelList->GetAllChildren())
    {
        if (const UChannelStatusWidget* ChannelStatusWidget = Cast<UChannelStatusWidget>(Child))
        {
            ChannelStatusWidget->UpdateSelection(CurrentChannel);
        }
    }
    OnChannelStatusClicked.Broadcast(CurrentChannel);
}

void UChannelListWidget::OnChannelsUpdated(const TArray<UChatChannel*>& InChannels)
{
    if (!ChannelList)
    {
        return;
    }

    if (!CurrentChannel)
    {
        // Select first channel. TODO support channel pagination
        if (InChannels.Num() > 0)
        {
            CurrentChannel = InChannels[0];
            OnChannelStatusClicked.Broadcast(CurrentChannel);
        }
    }

    ChannelList->ClearChildren();
    for (UChatChannel* InChannel : InChannels)
    {
        UChannelStatusWidget* Widget = CreateWidget<UChannelStatusWidget>(this, ChannelStatusWidgetClass);
        Widget->Setup(InChannel);
        Widget->OnChannelStatusButtonClicked.AddDynamic(this, &UChannelListWidget::ChannelStatusClicked);
        ChannelList->AddChild(Widget);
        Widget->UpdateSelection(CurrentChannel);
    }
}
