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
    if (Button)
    {
        Button->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::Box;
        Button->WidgetStyle.Pressed.DrawAs = ESlateBrushDrawType::Box;
        Button->WidgetStyle.Hovered.DrawAs = ESlateBrushDrawType::Box;
    }
}

void UChannelListHeaderWidget::OnButtonClick()
{
    ClientContext->SelectChannel(Client->NewChat());
}
