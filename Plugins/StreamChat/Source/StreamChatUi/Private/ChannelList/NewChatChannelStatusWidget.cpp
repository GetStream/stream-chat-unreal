// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ChannelList/NewChatChannelStatusWidget.h"

bool UNewChatChannelStatusWidget::IsForChannel(const UChatChannel* ChatChannel) const
{
    return ChatChannel == nullptr;
}
