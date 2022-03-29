// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "contact/SelectedContactWidget.h"

#include "ThemeDataAsset.h"

void USelectedContactWidget::OnSetup()
{
}

void USelectedContactWidget::OnTheme()
{
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->SelectedContactBackgroundColor));
    }
}
