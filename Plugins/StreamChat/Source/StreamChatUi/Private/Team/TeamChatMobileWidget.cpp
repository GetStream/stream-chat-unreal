// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Team/TeamChatMobileWidget.h"

#include "ThemeDataAsset.h"

UTeamChatMobileWidget::UTeamChatMobileWidget()
{
    bWantsTheme = true;
}

void UTeamChatMobileWidget::Setup(UStreamChatClientComponent* InClient)
{
    if (ClientContextWidget)
    {
        ClientContextWidget->Setup(InClient);
    }

    Super::Setup();
}

void UTeamChatMobileWidget::OnPreConstruct()
{
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

    Container->SetBrushColor(Theme->GetPaletteColor(Theme->ChannelListContainerBackgroundColor));
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

    Container->SetBrushColor(Theme->GetPaletteColor(Theme->MessageListContainerBackgroundColor));
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
