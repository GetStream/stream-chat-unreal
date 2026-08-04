// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

// Fill out your copyright notice in the Description page of Project Settings.

#include "Context/ClientContextWidget.h"

#include "WidgetUtil.h"

UClientContextWidget::UClientContextWidget()
{
    // This context wraps a whole screen, so it is the one that should clear the notch and the home
    // indicator. Nested contexts leave bApplySafeAreaPadding off to avoid insetting twice.
    bApplySafeAreaPadding = true;
}

void UClientContextWidget::Setup(UStreamChatClientComponent* InClient)
{
    Client = InClient;
}

UClientContextWidget* UClientContextWidget::Get(const UWidget* Widget)
{
    return WidgetUtil::GetTypedParentWidget<UClientContextWidget>(Widget);
}

UStreamChatClientComponent* UClientContextWidget::GetClient(const UWidget* Widget)
{
    if (const UClientContextWidget* Context = Get(Widget))
    {
        return Context->Client;
    }
    return nullptr;
}

UStreamChatClientComponent* UClientContextWidget::GetClient() const
{
    return Client;
}

void UClientContextWidget::SelectChannel(UChatChannel* Channel)
{
    if (SelectedChannel == Channel)
    {
        return;
    }

    SelectedChannel = Channel;
    OnChannelSelected.Broadcast(Channel);
}
