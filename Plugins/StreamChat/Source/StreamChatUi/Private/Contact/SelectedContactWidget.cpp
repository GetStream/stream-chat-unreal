// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Contact/SelectedContactWidget.h"

#include "ThemeDataAsset.h"

USelectedContactWidget::USelectedContactWidget()
{
}

void USelectedContactWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (Border)
    {
        Border->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->SelectedContactBackgroundColor));
    }
}
