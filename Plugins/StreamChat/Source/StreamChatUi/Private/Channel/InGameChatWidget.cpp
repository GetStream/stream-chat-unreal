// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/InGameChatWidget.h"

void UInGameChatWidget::Setup(UChatChannel* InChannel)
{
    Channel = InChannel;

    Super::Setup();
}

void UInGameChatWidget::OnSetup()
{
    if (FadingMessageList)
    {
        FadingMessageList->Setup(Channel);
    }
}
