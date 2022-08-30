// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Team/TeamChatWidget.h"

#include "ThemeDataAsset.h"

UTeamChatWidget::UTeamChatWidget()
{
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
    if (ClientContextWidget)
    {
        ClientContextWidget->OnChannelSelected.AddUniqueDynamic(this, &UTeamChatWidget::ChannelSelected);
    }
}

void UTeamChatWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (MessageListContainer)
    {
        MessageListContainer->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->MessageListContainerBackgroundColor));
    }
    if (ChannelListContainer)
    {
        ChannelListContainer->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->ChannelListContainerBackgroundColor));
    }
    if (Divider)
    {
        Divider->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->TeamChatDividerColor));
    }
}

void UTeamChatWidget::ChannelSelected(UChatChannel* SelectedChannel)
{
    if (!MessageListContainer)
    {
        return;
    }

    MessageListContainer->ClearChildren();
    if (SelectedChannel->IsValid())
    {
        UChannelWidget* Widget = CreateWidget<UChannelWidget>(this, ChannelWidgetClass);
        Widget->Setup(SelectedChannel);
        MessageListContainer->AddChild(Widget);
    }
    else
    {
        // Invalid channel means new chat
        UNewChatWidget* Widget = CreateWidget<UNewChatWidget>(this, NewChatWidgetClass);
        MessageListContainer->AddChild(Widget);
    }
}
