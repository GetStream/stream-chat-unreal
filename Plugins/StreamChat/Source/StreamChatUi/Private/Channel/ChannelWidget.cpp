// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelWidget.h"

#include "ThemeDataAsset.h"

UChannelWidget::UChannelWidget()
{
}

void UChannelWidget::Setup(UChatChannel* InChannel)
{
    MyChannel = InChannel;

    Super::Setup();
}

void UChannelWidget::OnSetup()
{
    if (OwnedChannelContext)
    {
        OwnedChannelContext->Setup(MyChannel);
    }
}

void UChannelWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (HeaderDivider)
    {
        HeaderDivider->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->ChannelHeaderDividerColor));
    }
}
