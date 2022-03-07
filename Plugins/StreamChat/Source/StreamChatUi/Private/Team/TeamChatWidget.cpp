// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Team/TeamChatWidget.h"

#include "ThemeDataAsset.h"

UTeamChatWidget::UTeamChatWidget()
{
    bWantsTheme = true;
}

void UTeamChatWidget::Setup(UStreamChatClientComponent* InClient)
{
    if (ClientContextWidget)
    {
        ClientContextWidget->Setup(InClient);
    }

    Super::Setup();
}

void UTeamChatWidget::OnSetup()
{
    if (MessageListContainer)
    {
        MessageListContainer->SetPadding({0.f});
    }
    if (ChannelList)
    {
        ChannelList->OnChannelStatusClicked.AddDynamic(this, &UTeamChatWidget::ChannelSelected);
    }
}

void UTeamChatWidget::OnTheme()
{
    if (MessageListContainer)
    {
        MessageListContainer->SetBrushColor(Theme->GetPaletteColor(Theme->MessageListContainerBackgroundColor));
    }
    if (ChannelListContainer)
    {
        ChannelListContainer->SetBrushColor(Theme->GetPaletteColor(Theme->ChannelListContainerBackgroundColor));
    }
    if (Divider)
    {
        Divider->SetColorAndOpacity(Theme->GetPaletteColor(Theme->TeamChatDividerColor));
    }
}

void UTeamChatWidget::ChannelSelected(UChatChannel* SelectedChannel)
{
    if (!MessageListContainer)
    {
        return;
    }

    MessageListContainer->ClearChildren();
    UChannelWidget* Widget = CreateWidget<UChannelWidget>(this, ChannelWidgetClass);
    Widget->Setup(SelectedChannel);
    MessageListContainer->AddChild(Widget);
}
