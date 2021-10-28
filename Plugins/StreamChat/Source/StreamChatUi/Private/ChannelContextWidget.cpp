// Fill out your copyright notice in the Description page of Project Settings.

#include "ChannelContextWidget.h"

#include "WidgetUtil.h"

UChatChannel* UChannelContextWidget::GetChannel(UWidget* Widget)
{
    return WidgetUtil::GetTypedParentWidget<UChannelContextWidget>(Widget)->Channel;
}
