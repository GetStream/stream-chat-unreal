// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// Fill out your copyright notice in the Description page of Project Settings.

#include "Context/ClientContextWidget.h"

#include "WidgetUtil.h"

void UClientContextWidget::Setup(UStreamChatClientComponent* InClient)
{
    Client = InClient;
}

UStreamChatClientComponent* UClientContextWidget::GetClient(const UWidget* Widget)
{
    if (const UClientContextWidget* Context = WidgetUtil::GetTypedParentWidget<UClientContextWidget>(Widget))
    {
        return Context->Client;
    }
    return nullptr;
}

UStreamChatClientComponent* UClientContextWidget::GetClient() const
{
    return Client;
}
