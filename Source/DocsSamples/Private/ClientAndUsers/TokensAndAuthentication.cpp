// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"
#include "User/User.h"
namespace
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/tokens_and_authentication/?language=unreal#developer-tokens
void DeveloperTokens()
{
    const FUser User{TEXT("john")};
    const FString Token = Client->DevToken(User.Id);
    Client->ConnectUser(
        User,
        Token,
        [](const FOwnUser& UserRef)
        {
            // Connection established
        });
}

}    // namespace
