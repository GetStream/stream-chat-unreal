// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"
#include "User/User.h"
namespace
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/logout/?language=unreal
void LogOut()
{
    Client->DisconnectUser();

    const FString Token{TEXT("CHAT_USER_TOKEN")};
    const FUser OtherUser{TEXT("tommaso")};
    Client->ConnectUser(
        OtherUser,
        Token,
        [](const FOwnUser& UserRef)
        {
            // New connection established
        });
}

}    // namespace
