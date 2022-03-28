// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelListHeaderWidget.h"

#include "Context/ClientContextWidget.h"
#include "ThemeDataAsset.h"

UChannelListHeaderWidget::UChannelListHeaderWidget()
{
    bAutoSetup = true;
    bWantsTheme = true;
    bWantsClient = true;
}

void UChannelListHeaderWidget::OnSetup()
{
    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UChannelListHeaderWidget::OnButtonClick);
    }
}

void UChannelListHeaderWidget::OnTheme()
{
    if (Icon)
    {
        Icon->SetColorAndOpacity(Theme->GetPaletteColor(Theme->ChannelListHeaderIconColor));
    }
}

void UChannelListHeaderWidget::OnButtonClick()
{
    if (const UClientContextWidget* Context = UClientContextWidget::Get(this))
    {
        Context->OnNewChat.Broadcast();
    }
}
