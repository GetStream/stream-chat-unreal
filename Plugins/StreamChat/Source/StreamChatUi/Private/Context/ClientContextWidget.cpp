// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// Fill out your copyright notice in the Description page of Project Settings.

#include "Context/ClientContextWidget.h"

#include "WidgetUtil.h"

void UClientContextWidget::Setup(UStreamChatClientComponent* InClient)
{
    Client = InClient;
}

UStreamChatClientComponent* UClientContextWidget::GetClient(UWidget* Widget)
{
    return WidgetUtil::GetTypedParentWidget<UClientContextWidget>(Widget)->Client;
}

UStreamChatClientComponent* UClientContextWidget::GetClient() const
{
    return Client;
}
