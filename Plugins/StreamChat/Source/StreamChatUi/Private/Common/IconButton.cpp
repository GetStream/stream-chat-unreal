// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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
    if (Button && Icon)
    {
        if (bInEnabled)
        {
            Button->SetStyle(EnabledStyle);
            Icon->SetBrushFromTexture(IconTexture);
        }
        else
        {
            Button->SetStyle(DisabledStyle);
            Icon->SetBrushFromTexture(DisabledIconTexture ? DisabledIconTexture : IconTexture);
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

void UIconButton::SetIconPaletteColor(const FName& InPaletteColor)
{
    if (Icon && Theme)
    {
        Icon->SetColorAndOpacity(Theme->GetPaletteColor(InPaletteColor));
    }
}

void UIconButton::SetEnabledBackgroundColor(const FName& InPaletteColor)
{
    if (Theme)
    {
        const FSlateColor EnabledColor{Theme->GetPaletteColor(InPaletteColor)};
        EnabledStyle.Normal.TintColor = EnabledColor;
        EnabledStyle.Hovered.TintColor = EnabledColor;
        EnabledStyle.Pressed.TintColor = EnabledColor;
    }
}

void UIconButton::SetDisabledBackgroundColor(const FName& InPaletteColor)
{
    if (Theme)
    {
        const FSlateColor DisabledColor{Theme->GetPaletteColor(InPaletteColor)};
        DisabledStyle.Normal.TintColor = DisabledColor;
        DisabledStyle.Hovered.TintColor = DisabledColor;
        DisabledStyle.Pressed.TintColor = DisabledColor;
    }
}

void UIconButton::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    OnTheme();
}

void UIconButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (Button)
    {
        Button->OnClicked.AddUniqueDynamic(this, &UIconButton::OnButtonClicked);
    }
}

void UIconButton::OnTheme()
{
    SetIconPadding(IconPadding);
    SetIconPaletteColor(IconPaletteColor);
    SetEnabledBackgroundColor(EnabledBackgroundColor);
    SetDisabledBackgroundColor(DisabledBackgroundColor);
    SetEnabled(bEnabled);
}

void UIconButton::OnButtonClicked()
{
    if (bEnabled)
    {
        OnClicked.Broadcast();
    }
}
