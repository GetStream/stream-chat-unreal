// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ThemeDataAsset.h"

#include "Context/ThemeContextWidget.h"

UThemeDataAsset* UThemeDataAsset::Get(const UWidget* Widget)
{
    return UThemeContextWidget::GetTheme(Widget);
}
