// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelWidget.h"

#include "ThemeDataAsset.h"

UChannelWidget::UChannelWidget()
{
    bWantsTheme = true;
}

void UChannelWidget::Setup(UChatChannel* InChannel)
{
    Channel = InChannel;

    Super::Setup();
}

void UChannelWidget::OnSetup()
{
    if (OwnedChannelContext)
    {
        OwnedChannelContext->Setup(Channel);
    }
}

void UChannelWidget::OnTheme()
{
    if (HeaderDivider)
    {
        HeaderDivider->SetColorAndOpacity(Theme->GetPaletteColor(Theme->ChannelHeaderDividerColor));
    }
}
