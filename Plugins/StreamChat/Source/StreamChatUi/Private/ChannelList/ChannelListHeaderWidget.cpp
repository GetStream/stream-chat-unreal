// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelListHeaderWidget.h"

#include "ThemeDataAsset.h"

UChannelListHeaderWidget::UChannelListHeaderWidget()
{
    bWantsTheme = true;
}

void UChannelListHeaderWidget::OnTheme()
{
    if (Icon)
    {
        Icon->SetColorAndOpacity(Theme->GetPaletteColor(Theme->ChannelListHeaderIconColor));
    }
}
