// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Common/BackButton.h"

#include "Components/ButtonSlot.h"
#include "Context/ClientContextWidget.h"
#include "ThemeDataAsset.h"

UBackButton::UBackButton()
{
    bWantsTheme = true;
}

void UBackButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (Button)
    {
        Button->OnClicked.AddUniqueDynamic(this, &UBackButton::OnButtonClicked);
    }
}

void UBackButton::OnTheme()
{
    const FSlateColor Color{Theme->GetPaletteColor(Theme->BackButtonSelectedColor)};
    Button->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::NoDrawType;
    Button->WidgetStyle.Hovered.TintColor = Color;
    Button->WidgetStyle.Pressed.TintColor = Color;

    if (Icon)
    {
        Icon->SetBrushFromTexture(IconTexture, true);
        if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Icon->Slot))
        {
            ButtonSlot->SetPadding(IconPadding);
        }
    }
}

void UBackButton::OnButtonClicked()
{
    if (const UClientContextWidget* Context = UClientContextWidget::Get(this))
    {
        Context->OnBack.Broadcast();
    }
}
