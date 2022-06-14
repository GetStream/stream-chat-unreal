// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "Channel/Message.h"
#include "CoreMinimal.h"

namespace
{
UChatChannel* Channel = nullptr;
FUserRef SystemUser;

// https://getstream.io/chat/docs/unreal/silent_messages/?language=unreal
void SilentMessages()
{
    FMessage Message{TEXT("You completed your trip")};
    Message.User = SystemUser;
    Message.bIsSilent = true;
    Channel->SendMessage(Message);
}

}    // namespace
