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

UClientContextWidget* UStreamWidget::GetClientContext() const
{
    ensureMsgf(bConstructed, TEXT("The widget needs to have been added to a parent/viewport first"));
    if (!ClientContext)
    {
        ClientContext = UClientContextWidget::Get(this);
    }
    return ClientContext;
}

UStreamChatClientComponent* UStreamWidget::GetClient() const
{
    ensureMsgf(bConstructed, TEXT("The widget needs to have been added to a parent/viewport first"));
    if (!Client)
    {
        if (ClientContext)
        {
            Client = ClientContext->GetClient();
        }
        else
        {
            Client = UClientContextWidget::GetClient(this);
        }
    }
    return Client;
}

UChannelContextWidget* UStreamWidget::GetChannelContext() const
{
    ensureMsgf(bConstructed, TEXT("The widget needs to have been added to a parent/viewport first"));
    if (!ChannelContext)
    {
        ChannelContext = UChannelContextWidget::Get(this);
    }
    return ChannelContext;
}

UChatChannel* UStreamWidget::GetChannel() const
{
    ensureMsgf(bConstructed, TEXT("The widget needs to have been added to a parent/viewport first"));
    if (!Channel)
    {
        if (ChannelContext)
        {
            Channel = ChannelContext->GetChannel();
        }
        else
        {
            Channel = UChannelContextWidget::GetChannel(this);
        }
    }
    return Channel;
}

UThemeDataAsset* UStreamWidget::GetTheme() const
{
    ensureMsgf(bConstructed, TEXT("The widget needs to have been added to a parent/viewport first"));
    if (!Theme)
    {
        Theme = UThemeDataAsset::Get(this);
        Theme = Theme ? Theme : GetDefault<UThemeContextWidget>()->Theme;
    }
    return Theme;
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

void UStreamWidget::NativePreConstruct()
{
    bConstructed = true;
    Super::NativePreConstruct();
}
