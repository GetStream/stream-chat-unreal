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

UWidget* UChannelListWidget::CreateChannelWidget(UChatChannel* const& InChannel)
{
    if (InChannel->IsValid())
    {
        UChannelStatusWidget* Widget = CreateWidget<USummaryChannelStatusWidget>(this, ChannelStatusWidgetClass);
        Widget->Setup(InChannel);

        return Widget;
    }

    UChannelStatusWidget* Widget = CreateWidget<UNewChatChannelStatusWidget>(this, NewChatChannelStatusWidgetClass);
    Widget->Setup(nullptr);
    return Widget;
}

void UChannelListWidget::OnClient()
{
    Client->ChannelsUpdated.AddDynamic(this, &UChannelListWidget::OnChannelsUpdated);

    ClientContext->OnBack.AddDynamic(this, &UChannelListWidget::OnBack);
    ClientContext->OnChannelSelected.AddDynamic(this, &UChannelListWidget::OnChannelSelected);

    if (ListView)
    {
        PaginateListWidget = SNew(SPaginateListWidget<UChatChannel*>)
                                 .Limit(Limit)
                                 .PaginationDirection(EPaginationDirection::Bottom)
                                 .ListItemsSource(&Client->GetChannels())
                                 .CreateListViewWidget_UObject(this, &UChannelListWidget::CreateChannelWidget)
                                 .DoPaginate_UObject(this, &UChannelListWidget::Paginate);
        ListView->SetContent(PaginateListWidget.ToSharedRef());
    }
}

void UChannelListWidget::OnBack()
{
    // if (IsNewChatActive())
    {
        Client->CancelNewChat();
        RebuildChannelList();
    }
}

void UChannelListWidget::OnTheme()
{
    if (Divider)
    {
        Divider->SetColorAndOpacity(Theme->GetPaletteColor(Theme->TeamChatDividerColor));
    }
}

void UChannelListWidget::RebuildChannelList()
{
    PaginateListWidget->RebuildList();
}

void UChannelListWidget::OnChannelSelected(UChatChannel* ClickedChannel)
{
    if (ClickedChannel == CurrentChannel)
    {
        return;
    }
    CurrentChannel = ClickedChannel;

    if (CurrentChannel)
    {
        PaginateListWidget->RequestScrollIntoView(CurrentChannel);
    }
}

void UChannelListWidget::OnChannelsUpdated(const TArray<UChatChannel*>&)
{
    RebuildChannelList();

    if (!CurrentChannel && bAutoSelectFirstChannel && !Client->GetChannels().IsEmpty())
    {
        // Select first channel.
        CurrentChannel = Client->GetChannels()[0];
        ClientContext->SelectChannel(CurrentChannel);
    }
}
