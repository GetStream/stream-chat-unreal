// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "StreamWidget.h"

#include "Context/ChannelContextWidget.h"
#include "ThemeDataAsset.h"

void UStreamWidget::Setup()
{
    OnSetup();
}

bool UStreamWidget::Initialize()
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

void UStreamWidget::NativeConstruct()
{
}

void UStreamWidget::NativePreConstruct()
{
    if (bWantsTheme)
    {
        if (const UThemeDataAsset* Theme = UThemeDataAsset::Get(this))
        {
            OnTheme(Theme);
            OnTheme_BP(Theme);
        }
    }
    if (bWantsChannel)
    {
        ChannelContext = UChannelContextWidget::Get(this);
        if (Channel)
        {
            Channel = ChannelContext->GetChannel();
            if (ChannelContext)
            {
                OnChannel();
                OnChannel_BP();
            }
        }
    }
}
