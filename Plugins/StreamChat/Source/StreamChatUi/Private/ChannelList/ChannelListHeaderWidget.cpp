// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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
        FButtonStyle Style = Button->GetStyle();
        Style.Normal.DrawAs = ESlateBrushDrawType::Box;
        Style.Pressed.DrawAs = ESlateBrushDrawType::Box;
        Style.Hovered.DrawAs = ESlateBrushDrawType::Box;
        Button->SetStyle(Style);
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
