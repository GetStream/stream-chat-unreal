// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ThemeDataAsset.h"

#include "Context/ThemeContextWidget.h"

const FLinearColor& UThemeDataAsset::GetPaletteColor(const FName& Name) const
{
    if (const FLinearColor* Color = Palette.Find(Name))
    {
        return *Color;
    }

    return FLinearColor::Red;
}

UThemeDataAsset* UThemeDataAsset::Get(const UWidget* Widget)
{
    return UThemeContextWidget::GetTheme(Widget);
}
