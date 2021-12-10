// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Common/IconButton.h"

#include "Components/ButtonSlot.h"
#include "ThemeDataAsset.h"

UIconButton::UIconButton()
{
    bWantsTheme = true;
}

void UIconButton::SetEnabled(const bool bInEnabled)
{
    bEnabled = bInEnabled;
    if (Button)
    {
        if (bInEnabled)
        {
            Button->SetStyle(EnabledStyle);
        }
        else
        {
            Button->SetStyle(DisabledStyle);
        }
    }
}

void UIconButton::SetIconFromTexture(UTexture2D* Texture)
{
    if (Icon)
    {
        Icon->SetBrushFromTexture(Texture, true);
    }
}

void UIconButton::SetIconPadding(const FMargin& InIconPadding)
{
    if (Icon)
    {
        if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Icon->Slot))
        {
            ButtonSlot->SetPadding(InIconPadding);
        }
    }
}

void UIconButton::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    SetIconFromTexture(IconTexture);
    SetIconPadding(IconPadding);
    SetEnabled(bEnabled);
}

void UIconButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (Button)
    {
        Button->OnClicked.AddUniqueDynamic(this, &UIconButton::OnButtonClicked);
    }
}

void UIconButton::OnTheme(const UThemeDataAsset* Theme)
{
    const FSlateColor EnabledColor{Theme->GetPaletteColor(Theme->IconButtonEnabledBackgroundColor)};
    const FSlateColor DisabledColor{Theme->GetPaletteColor(Theme->IconButtonDisabledBackgroundColor)};
    EnabledStyle.Normal.TintColor = EnabledColor;
    EnabledStyle.Hovered.TintColor = EnabledColor;
    EnabledStyle.Pressed.TintColor = EnabledColor;
    DisabledStyle.Normal.TintColor = DisabledColor;
    DisabledStyle.Hovered.TintColor = DisabledColor;
    DisabledStyle.Pressed.TintColor = DisabledColor;
}

void UIconButton::OnButtonClicked()
{
    if (bEnabled)
    {
        OnClicked.Broadcast();
    }
}
