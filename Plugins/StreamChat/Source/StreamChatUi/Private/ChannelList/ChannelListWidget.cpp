// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelListWidget.h"

#include "ChannelList/SummaryChannelStatusWidget.h"
#include "Context/ClientContextWidget.h"
#include "StreamChatClientComponent.h"
#include "ThemeDataAsset.h"

UChannelListWidget::UChannelListWidget()
{
    bWantsClient = true;
    bWantsTheme = true;
    Limit = 10;
}

void UChannelListWidget::NativeDestruct()
{
    if (Client)
    {
        Client->ChannelsUpdated.RemoveDynamic(this, &UChannelListWidget::OnChannelsUpdated);
    }
    Super::NativeDestruct();
}

void UChannelListWidget::Paginate(const EPaginationDirection Direction, const TFunction<void()> Callback)
{
    Client->QueryAdditionalChannels(Limit, Callback);
}

void UChannelListWidget::OnClient()
{
    Client->ChannelsUpdated.AddDynamic(this, &UChannelListWidget::OnChannelsUpdated);

    RepopulateChannelList();

    if (UClientContextWidget* Context = UClientContextWidget::Get(this))
    {
        Context->OnBack.AddDynamic(this, &UChannelListWidget::OnBack);
        Context->OnNewChat.AddDynamic(this, &UChannelListWidget::OnNewChat);
    }
}

void UChannelListWidget::OnBack()
{
    if (IsNewChatActive())
    {
        CurrentChannel = NewChatPreviousChannel;
        OnChannelStatusClicked.Broadcast(CurrentChannel);
        NewChatPreviousChannel = nullptr;
        RepopulateChannelList();
    }
}

void UChannelListWidget::OnNewChat()
{
    if (IsNewChatActive())
    {
        return;
    }
    NewChatPreviousChannel = CurrentChannel;
    CurrentChannel = nullptr;
    OnChannelStatusClicked.Broadcast(CurrentChannel);
    RepopulateChannelList();
}

void UChannelListWidget::OnTheme()
{
    if (Divider)
    {
        Divider->SetColorAndOpacity(Theme->GetPaletteColor(Theme->TeamChatDividerColor));
    }
}

void UChannelListWidget::RepopulateChannelList()
{
    OnChannelsUpdated(Client->GetChannels());
}

bool UChannelListWidget::IsNewChatActive() const
{
    return NewChatPreviousChannel != nullptr;
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
    if (!ScrollBox || InChannels.Num() == 0)
    {
        return;
    }

    if (!CurrentChannel && !IsNewChatActive() && bAutoSelectFirstChannel)
    {
        // Select first channel. TODO support channel pagination
        CurrentChannel = Client->GetChannels()[0];
        OnChannelStatusClicked.Broadcast(CurrentChannel);
    }

    TArray<UWidget*> Widgets;
    Widgets.Reserve(InChannels.Num() + (IsNewChatActive() ? 1 : 0));

    if (IsNewChatActive())
    {
        UChannelStatusWidget* Widget = CreateWidget<UNewChatChannelStatusWidget>(this, NewChatChannelStatusWidgetClass);
        Widget->Setup(nullptr);
        Widget->OnChannelStatusButtonClicked.AddDynamic(this, &UChannelListWidget::ChannelStatusClicked);
        Widgets.Add(Widget);
    }

    for (UChatChannel* InChannel : InChannels)
    {
        UChannelStatusWidget* Widget = CreateWidget<USummaryChannelStatusWidget>(this, ChannelStatusWidgetClass);
        Widget->Setup(InChannel);
        Widget->OnChannelStatusButtonClicked.AddDynamic(this, &UChannelListWidget::ChannelStatusClicked);
        Widgets.Add(Widget);
    }
    SetChildren(Widgets);

    // Needs theme so has to happen after adding to widget hierarchy
    for (UWidget* Widget : Widgets)
    {
        UChannelStatusWidget* StatusWidget = static_cast<UChannelStatusWidget*>(Widget);
        StatusWidget->UpdateSelection(CurrentChannel);
        if (StatusWidget->IsForChannel(CurrentChannel))
        {
            ScrollBox->ScrollWidgetIntoView(StatusWidget);
        }
    }
}
