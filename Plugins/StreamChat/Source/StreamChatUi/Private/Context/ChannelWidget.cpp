// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelWidget.h"

void UChannelWidget::Setup(UChatChannel* InChannel)
{
    Channel = InChannel;

    Super::Setup();
}

void UChannelWidget::OnSetup()
{
    if (ChannelContext)
    {
        ChannelContext->Setup(Channel);
    }
}
