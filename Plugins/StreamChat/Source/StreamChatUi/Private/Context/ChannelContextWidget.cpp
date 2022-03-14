// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

// Fill out your copyright notice in the Description page of Project Settings.

#include "Context/ChannelContextWidget.h"

#include "WidgetUtil.h"

void UChannelContextWidget::Setup(UChatChannel* InChannel)
{
    Channel = InChannel;
}

UChannelContextWidget* UChannelContextWidget::Get(const UWidget* Widget)
{
    if (!Widget || Widget->IsDesignTime())
    {
        return nullptr;
    }

    UChannelContextWidget* ContextWidget = WidgetUtil::GetTypedParentWidget<UChannelContextWidget>(Widget);
    if (ensureAlwaysMsgf(ContextWidget, TEXT("Need to wrap the channel UI with a ChannelContextWidget")))
    {
        return ContextWidget;
    }
    return nullptr;
}

UChatChannel* UChannelContextWidget::GetChannel(const UWidget* Widget)
{
    if (!Widget || Widget->IsDesignTime())
    {
        return nullptr;
    }

    if (const UChannelContextWidget* ContextWidget = Get(Widget))
    {
        if (ensureAlwaysMsgf(ContextWidget->Channel, TEXT("ChannelContextWidget needs to be initialized with a ChatChannel")))
        {
            return ContextWidget->Channel;
        }
    }
    return nullptr;
}

UChatChannel* UChannelContextWidget::GetChannel() const
{
    return Channel;
}
