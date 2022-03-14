// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "StreamWidget.h"

#include "Context/ChannelContextWidget.h"
#include "Context/ClientContextWidget.h"
#include "Context/ThemeContextWidget.h"
#include "ThemeDataAsset.h"

void UStreamWidget::Setup()
{
    OnSetup();
}

bool UStreamWidget::Initialize()
{
    if (Super::Initialize())
    {
        if (IsDesignTime() || bAutoSetup)
        {
            OnSetup();
        }
        return true;
    }

    return false;
}

void UStreamWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UStreamWidget::NativePreConstruct()
{
    if (bWantsTheme)
    {
        Theme = UThemeDataAsset::Get(this);
        Theme = Theme ? Theme : GetDefault<UThemeContextWidget>()->Theme;
        OnTheme();
        OnTheme_BP();
    }
    if (bWantsClient)
    {
        Client = UClientContextWidget::GetClient(this);
        if (Client)
        {
            OnClient();
            OnClient_BP();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("bWantsClient is true, but widget doesn't have a ClientContextWidget in the parent tree."))
        }
    }
    if (bWantsChannel)
    {
        ChannelContext = UChannelContextWidget::Get(this);
        if (ChannelContext)
        {
            Channel = ChannelContext->GetChannel();
            if (ChannelContext)
            {
                OnChannel();
                OnChannel_BP();
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("bWantsChannel is true, but widget doesn't have a ChannelContextWidget in the parent tree."))
        }
    }
    OnPreConstruct();
    Super::NativePreConstruct();
}
