// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "StreamUserWidget.h"

#include "ThemeDataAsset.h"

void UStreamUserWidget::Setup()
{
    OnSetup();
}

bool UStreamUserWidget::Initialize()
{
    if (Super::Initialize())
    {
        if (IsDesignTime())
        {
            OnSetup();
        }
        return true;
    }

    return false;
}

void UStreamUserWidget::NativeConstruct()
{
}

void UStreamUserWidget::NativePreConstruct()
{
    if (WantsTheme())
    {
        if (UThemeDataAsset* Theme = UThemeDataAsset::Get(this))
        {
            OnTheme(Theme);
        }
    }
}
