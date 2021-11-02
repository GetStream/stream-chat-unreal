// Fill out your copyright notice in the Description page of Project Settings.

#include "Context/ClientContextWidget.h"

#include "WidgetUtil.h"

UStreamChatClientComponent* UClientContextWidget::GetClient(UWidget* Widget)
{
    return WidgetUtil::GetTypedParentWidget<UClientContextWidget>(Widget)->Client;
}
