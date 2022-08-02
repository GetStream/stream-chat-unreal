// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelStatusWidget.h"

#include "Context/ClientContextWidget.h"
#include "ThemeDataAsset.h"
#include "UiBlueprintLibrary.h"

UChannelStatusWidget::UChannelStatusWidget()
{
}

void UChannelStatusWidget::Setup(UChatChannel* InChannel)
{
    StatusChannel = InChannel;

    Super::Setup();
}

void UChannelStatusWidget::OnSetup()
{
    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UChannelStatusWidget::OnButtonClicked);
    }
}

void UChannelStatusWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (GetClientContext())
    {
        GetClientContext()->OnChannelSelected.AddDynamic(this, &UChannelStatusWidget::UpdateSelection);
        UpdateSelection(GetClientContext()->SelectedChannel);
    }

    // Theming
    NormalStyle.Normal.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonNormalBackgroundColor)};
    NormalStyle.Pressed.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonSelectedBackgroundColor)};
    NormalStyle.Hovered.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonHoveredBackgroundColor)};
    SelectedStyle.Normal.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonSelectedBackgroundColor)};
    SelectedStyle.Pressed.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonSelectedBackgroundColor)};
    SelectedStyle.Hovered.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->HeaderButtonSelectedBackgroundColor)};
    if (Button)
    {
        Button->SetStyle(NormalStyle);
    }

    if (Divider)
    {
        Divider->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->ChannelStatusDividerColor));
    }

    if (TitleTextBlock)
    {
        TitleTextBlock->SetColorAndOpacity(GetTitleColor());
    }
}
void UChannelStatusWidget::NativeDestruct()
{
    if (GetClientContext())
    {
        GetClientContext()->OnChannelSelected.RemoveDynamic(this, &UChannelStatusWidget::UpdateSelection);
    }
    if (Button)
    {
        Button->OnClicked.RemoveDynamic(this, &UChannelStatusWidget::OnButtonClicked);
    }
    Super::NativeDestruct();
}

void UChannelStatusWidget::UpdateSelection(UChatChannel* SelectedChannel)
{
    if (!Button)
    {
        return;
    }

    if (IsForChannel(SelectedChannel))
    {
        Button->SetStyle(SelectedStyle);
    }
    else
    {
        Button->SetStyle(NormalStyle);
    }
}

bool UChannelStatusWidget::IsForChannel(const UChatChannel* QueryChannel) const
{
    return false;
}

FLinearColor UChannelStatusWidget::GetTitleColor()
{
    return GetTheme()->GetPaletteColor(GetTheme()->ChannelStatusTitleTextColor);
}

void UChannelStatusWidget::OnButtonClicked()
{
    OnChannelStatusButtonClickedNative.Broadcast(StatusChannel);
    OnChannelStatusButtonClicked.Broadcast(StatusChannel);
    GetClientContext()->SelectChannel(StatusChannel);
}
