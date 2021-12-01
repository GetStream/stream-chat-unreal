// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// Fill out your copyright notice in the Description page of Project Settings.

#include "Context/ThemeContextWidget.h"

#include "WidgetUtil.h"

FLinearColor UThemeContextWidget::GetTheme(UWidget* Widget)
{
    return WidgetUtil::GetTypedParentWidget<UThemeContextWidget>(Widget)->Theme;
}
