// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "StreamUserWidget.h"

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
