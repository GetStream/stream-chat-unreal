// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Contact/SelectedContactWidget.h"

#include "ThemeDataAsset.h"

void USelectedContactWidget::OnTheme()
{
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->SelectedContactBackgroundColor));
    }
}
