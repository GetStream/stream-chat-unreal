// Fill out your copyright notice in the Description page of Project Settings.

#include "Context/ChannelContextWidget.h"

#include "WidgetUtil.h"

UChatChannel* UChannelContextWidget::GetChannel(UWidget* Widget)
{
    if (UChannelContextWidget* ContextWidget = WidgetUtil::GetTypedParentWidget<UChannelContextWidget>(Widget);
        ensureAlwaysMsgf(ContextWidget, TEXT("Need to wrap the channel UI with a ChannelContextWidget")))
    {
        if (ensureAlwaysMsgf(
                ContextWidget->Channel, TEXT("ChannelContextWidget needs to be initialized with a ChatChannel")))
        {
            return ContextWidget->Channel;
        }
    }
    return nullptr;
}
