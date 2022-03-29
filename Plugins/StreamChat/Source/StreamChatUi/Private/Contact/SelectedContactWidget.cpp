// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Contact/SelectedContactWidget.h"

#include "ThemeDataAsset.h"

USelectedContactWidget::USelectedContactWidget()
{
    bWantsTheme = true;
}

void USelectedContactWidget::OnTheme()
{
    Super::OnTheme();
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->SelectedContactBackgroundColor));
    }
}
