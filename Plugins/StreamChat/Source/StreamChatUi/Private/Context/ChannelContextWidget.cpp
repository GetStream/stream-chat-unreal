// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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

UChannelContextWidget* UChannelContextWidget::TryGet(const UWidget* Widget)
{
    if (!Widget || Widget->IsDesignTime())
    {
        return nullptr;
    }

    return WidgetUtil::GetTypedParentWidget<UChannelContextWidget>(Widget);
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

UChatChannel* UChannelContextWidget::TryGetChannel(const UWidget* Widget)
{
    if (!Widget || Widget->IsDesignTime())
    {
        return nullptr;
    }

    if (const UChannelContextWidget* ContextWidget = TryGet(Widget))
    {
        return ContextWidget->Channel;
    }
    return nullptr;
}

UChatChannel* UChannelContextWidget::GetChannel() const
{
    return Channel;
}

void UChannelContextWidget::OpenThread(const FMessage& ParentMessage)
{
    if (ParentMessage.Id.IsEmpty())
    {
        return;
    }

    ThreadParentMessage = ParentMessage;
    bThreadOpen = true;

    // A channel query returns channel history and not thread history, so the replies have to be
    // fetched when the thread is opened. Doing it here means every widget that follows
    // OnThreadChanged gets a populated thread without each of them asking for it.
    if (Channel)
    {
        Channel->QueryReplies(ParentMessage);
    }

    OnThreadChanged.Broadcast(true, ThreadParentMessage);
}

void UChannelContextWidget::CloseThread()
{
    if (!bThreadOpen)
    {
        return;
    }

    const FMessage ClosedParent = ThreadParentMessage;
    bThreadOpen = false;
    ThreadParentMessage = FMessage{};

    OnThreadChanged.Broadcast(false, ClosedParent);
}

bool UChannelContextWidget::IsThreadOpen() const
{
    return bThreadOpen;
}

const FMessage& UChannelContextWidget::GetThreadParentMessage() const
{
    return ThreadParentMessage;
}
