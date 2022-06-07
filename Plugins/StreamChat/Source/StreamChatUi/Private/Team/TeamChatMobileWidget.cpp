// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Team/TeamChatMobileWidget.h"

#include "ThemeDataAsset.h"

UTeamChatMobileWidget::UTeamChatMobileWidget()
{
}

void UTeamChatMobileWidget::Setup(UStreamChatClientComponent* InClient)
{
    if (ClientContextWidget)
    {
        ClientContextWidget->Setup(InClient);
    }

    Super::Setup();
}

void UTeamChatMobileWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    SwitchToChannelList();
}

void UTeamChatMobileWidget::SwitchToChannelList()
{
    if (!Container)
    {
        return;
    }

    ClientContextWidget->OnBack.RemoveDynamic(this, &UTeamChatMobileWidget::SwitchToChannelList);
    ClientContextWidget->OnChannelSelected.AddDynamic(this, &UTeamChatMobileWidget::ChannelSelected);

    Container->ClearChildren();

    UChannelListWidget* ChannelListWidget = CreateWidget<UChannelListWidget>(this, ChannelListWidgetClass);
    ChannelListWidget->Setup();
    Container->AddChild(ChannelListWidget);

    Container->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->ChannelListContainerBackgroundColor));
}

void UTeamChatMobileWidget::SwitchToMessageList(UChatChannel* SelectedChannel)
{
    if (!Container)
    {
        return;
    }

    ClientContextWidget->OnBack.AddDynamic(this, &UTeamChatMobileWidget::SwitchToChannelList);
    ClientContextWidget->OnChannelSelected.RemoveDynamic(this, &UTeamChatMobileWidget::ChannelSelected);

    Container->ClearChildren();

    UChannelWidget* ChannelWidget = CreateWidget<UChannelWidget>(this, ChannelWidgetClass);
    ChannelWidget->Setup(SelectedChannel);
    Container->AddChild(ChannelWidget);

    Container->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->MessageListContainerBackgroundColor));
}

void UTeamChatMobileWidget::OnSetup()
{
    if (Container)
    {
        Container->SetPadding({0.f});
    }
}

void UTeamChatMobileWidget::ChannelSelected(UChatChannel* SelectedChannel)
{
    SwitchToMessageList(SelectedChannel);
}
