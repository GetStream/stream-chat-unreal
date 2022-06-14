// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"

namespace
{
UChatChannel* Channel;

// https://getstream.io/chat/docs/unreal/message_format/?language=unreal#open-graph-scraper
void OpenGraphScraper()
{
    const FMessage Message{TEXT("Check this bear out https://imgur.com/r/bears/4zmGbMN")};
    Channel->SendMessage(Message);
}

}    // namespace
