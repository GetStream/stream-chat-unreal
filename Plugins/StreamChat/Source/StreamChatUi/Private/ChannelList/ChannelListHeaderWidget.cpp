// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelListHeaderWidget.h"

#include "Context/ClientContextWidget.h"
#include "ThemeDataAsset.h"

void UChannelListHeaderWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (Icon)
    {
        Icon->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->ChannelListHeaderIconColor));
    }
    if (Button)
    {
        Button->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::Box;
        Button->WidgetStyle.Pressed.DrawAs = ESlateBrushDrawType::Box;
        Button->WidgetStyle.Hovered.DrawAs = ESlateBrushDrawType::Box;
    }
}

void UChannelListHeaderWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UChannelListHeaderWidget::OnButtonClick);
    }
}

void UChannelListHeaderWidget::NativeDestruct()
{
    if (Button)
    {
        Button->OnClicked.RemoveDynamic(this, &UChannelListHeaderWidget::OnButtonClick);
    }
    Super::NativeDestruct();
}

void UChannelListHeaderWidget::OnButtonClick()
{
    GetClientContext()->SelectChannel(GetClient()->NewChat());
}
