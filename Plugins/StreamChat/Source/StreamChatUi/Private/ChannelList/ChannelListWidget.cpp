// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelListWidget.h"

#include "ChannelList/SummaryChannelStatusWidget.h"
#include "Context/ClientContextWidget.h"
#include "StreamChatClientComponent.h"
#include "ThemeDataAsset.h"

UChannelListWidget::UChannelListWidget()
{
    Limit = 10;
}

void UChannelListWidget::NativeDestruct()
{
    if (GetClient())
    {
        GetClient()->ChannelsUpdated.RemoveDynamic(this, &UChannelListWidget::OnChannelsUpdated);
    }
    Super::NativeDestruct();
}

void UChannelListWidget::ReleaseSlateResources(const bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);

    PaginateListWidget.Reset();
}

void UChannelListWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    GetClient()->ChannelsUpdated.AddDynamic(this, &UChannelListWidget::OnChannelsUpdated);

    GetClientContext()->OnBack.AddDynamic(this, &UChannelListWidget::OnBack);
    GetClientContext()->OnChannelSelected.AddDynamic(this, &UChannelListWidget::OnChannelSelected);

    if (ListView)
    {
        PaginateListWidget = SNew(SPaginateListWidget<UChatChannel*>)
                                 .Limit(Limit)
                                 .PaginationDirection(EPaginationDirection::Bottom)
                                 .ListItemsSource(&GetClient()->GetChannels())
                                 .CreateListViewWidget_UObject(this, &UChannelListWidget::CreateChannelWidget)
                                 .DoPaginate_UObject(this, &UChannelListWidget::Paginate);
        ListView->SetContent(PaginateListWidget.ToSharedRef());
    }

    if (Divider)
    {
        Divider->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->TeamChatDividerColor));
    }
}

void UChannelListWidget::Paginate(const EPaginationDirection Direction, const TFunction<void()> Callback)
{
    GetClient()->QueryAdditionalChannels(Limit, Callback);
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
    Widget->Setup(InChannel);
    return Widget;
}

void UChannelListWidget::OnBack()
{
    // if (IsNewChatActive())
    {
        GetClient()->CancelNewChat();
        RebuildChannelList();
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

    if (!CurrentChannel && bAutoSelectFirstChannel && GetClient()->GetChannels().Num() > 0)
    {
        // Select first channel.
        CurrentChannel = GetClient()->GetChannels()[0];
        GetClientContext()->SelectChannel(CurrentChannel);
    }
}
