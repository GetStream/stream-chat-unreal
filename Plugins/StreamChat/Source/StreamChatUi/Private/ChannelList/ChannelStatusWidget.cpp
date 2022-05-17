// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/ChannelStatusWidget.h"

#include "Context/ClientContextWidget.h"
#include "ThemeDataAsset.h"
#include "UiBlueprintLibrary.h"

UChannelStatusWidget::UChannelStatusWidget()
{
    bWantsTheme = true;
    bWantsClient = true;
}

void UChannelStatusWidget::Setup(UChatChannel* InChannel)
{
    Channel = InChannel;

    Super::Setup();
}

void UChannelStatusWidget::OnClient()
{
    ClientContext->OnChannelSelected.AddDynamic(this, &UChannelStatusWidget::UpdateSelection);
    UpdateSelection(ClientContext->SelectedChannel);
}

void UChannelStatusWidget::NativeDestruct()
{
    if (ClientContext)
    {
        ClientContext->OnChannelSelected.RemoveDynamic(this, &UChannelStatusWidget::UpdateSelection);
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

void UChannelStatusWidget::OnSetup()
{
    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UChannelStatusWidget::OnButtonClicked);
    }
}

void UChannelStatusWidget::OnTheme()
{
    NormalStyle.Normal.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonNormalBackgroundColor)};
    NormalStyle.Pressed.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonSelectedBackgroundColor)};
    NormalStyle.Hovered.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonHoveredBackgroundColor)};
    SelectedStyle.Normal.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonSelectedBackgroundColor)};
    SelectedStyle.Pressed.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonSelectedBackgroundColor)};
    SelectedStyle.Hovered.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->HeaderButtonSelectedBackgroundColor)};
    if (Button)
    {
        Button->SetStyle(NormalStyle);
    }

    if (Divider)
    {
        Divider->SetColorAndOpacity(Theme->GetPaletteColor(Theme->ChannelStatusDividerColor));
    }

    if (TitleTextBlock)
    {
        TitleTextBlock->SetColorAndOpacity(GetTitleColor());
    }
}

FLinearColor UChannelStatusWidget::GetTitleColor()
{
    return Theme->GetPaletteColor(Theme->ChannelStatusTitleTextColor);
}

void UChannelStatusWidget::OnButtonClicked()
{
    OnChannelStatusButtonClickedNative.Broadcast(Channel);
    OnChannelStatusButtonClicked.Broadcast(Channel);
    ClientContext->SelectChannel(Channel);
}
