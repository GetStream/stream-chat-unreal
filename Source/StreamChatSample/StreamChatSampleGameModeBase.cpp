// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "StreamChatSampleGameModeBase.h"

#include "StreamChatSampleHud.h"

AStreamChatSampleGameModeBase::AStreamChatSampleGameModeBase()
{
    HUDClass = AStreamChatSampleHud::StaticClass();
}
