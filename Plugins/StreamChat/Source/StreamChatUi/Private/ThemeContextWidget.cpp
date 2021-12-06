// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// Fill out your copyright notice in the Description page of Project Settings.

#include "Context/ThemeContextWidget.h"

#include "WidgetUtil.h"

UThemeContextWidget::UThemeContextWidget()
{
    static ConstructorHelpers::FObjectFinder<UThemeDataAsset> DefaultTheme(TEXT("/StreamChat/UI/Themes/LightTheme"));
    Theme = DefaultTheme.Object;
}

UThemeDataAsset* UThemeContextWidget::GetTheme(const UWidget* Widget)
{
    if (const UThemeContextWidget* Context = WidgetUtil::GetTypedParentWidget<UThemeContextWidget>(Widget))
    {
        return Context->Theme;
    }
    return nullptr;
}
