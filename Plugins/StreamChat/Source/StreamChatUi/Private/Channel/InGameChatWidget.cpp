// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/InGameChatWidget.h"

void UInGameChatWidget::Setup(UChatChannel* InChannel)
{
    MyChannel = InChannel;

    Super::Setup();
}

void UInGameChatWidget::OnSetup()
{
    if (FadingMessageList)
    {
        FadingMessageList->Setup(MyChannel);
    }
}
