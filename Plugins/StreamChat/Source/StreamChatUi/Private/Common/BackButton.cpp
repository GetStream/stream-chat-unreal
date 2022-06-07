// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Common/BackButton.h"

#include "Components/ButtonSlot.h"
#include "Context/ClientContextWidget.h"
#include "ThemeDataAsset.h"

UBackButton::UBackButton()
{
}

void UBackButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (Button)
    {
        Button->OnClicked.AddUniqueDynamic(this, &UBackButton::OnButtonClicked);
    }
}

void UBackButton::NativePreConstruct()
{
    Super::NativePreConstruct();
    const FSlateColor Color{GetTheme()->GetPaletteColor(GetTheme()->BackButtonSelectedColor)};
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
