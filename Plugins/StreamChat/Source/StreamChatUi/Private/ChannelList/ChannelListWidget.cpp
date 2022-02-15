// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelListWidget.h"

#include "StreamChatClientComponent.h"

UChannelListWidget::UChannelListWidget()
{
    bWantsClient = true;
}

void UChannelListWidget::Paginate(const EPaginationDirection Direction, const TFunction<void()> Callback)
{
    // Client->QueryChannels();
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

    for (UWidget* Child : ScrollBox->GetAllChildren())
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
    if (!ScrollBox)
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

    TArray<UWidget*> Widgets;
    Widgets.Reserve(InChannels.Num());
    for (UChatChannel* InChannel : InChannels)
    {
        UChannelStatusWidget* Widget = CreateWidget<UChannelStatusWidget>(this, ChannelStatusWidgetClass);
        Widget->Setup(InChannel);
        Widget->OnChannelStatusButtonClicked.AddDynamic(this, &UChannelListWidget::ChannelStatusClicked);
        Widgets.Add(Widget);
        Widget->UpdateSelection(CurrentChannel);
    }
    SetChildren(Widgets);
}
