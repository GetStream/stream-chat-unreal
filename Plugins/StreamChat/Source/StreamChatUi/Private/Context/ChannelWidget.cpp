// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelWidget.h"

void UChannelWidget::Setup(UChatChannel* InChannel)
{
    // Channel needs to be provided as this widget won't be under a ChannelContextProvider in the hierarchy
    Channel = InChannel;

    Super::Setup();
}

void UChannelWidget::OnSetup()
{
    if (OwnedChannelContext)
    {
        OwnedChannelContext->Setup(Channel);
    }
}
