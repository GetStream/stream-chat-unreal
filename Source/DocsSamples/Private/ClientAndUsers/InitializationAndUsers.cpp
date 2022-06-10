// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"
#include "User/User.h"
class UInitializationAndUsers final : public UObject
{
    UStreamChatClientComponent* Client = nullptr;

    // https://getstream.io/chat/docs/react/init_and_users/?language=unreal
    void CreateClient()
    {
        Client = CreateDefaultSubobject<UStreamChatClientComponent>(TEXT("Client"));
        Client->ApiKey = TEXT("kmajgxb2rk4p");
    }

    void ConnectingTheUser()
    {
        const FUser User{TEXT("john")};
        const FString Token{TEXT("{{ chat_user_token }}")};
        Client->ConnectUser(
            User,
            Token,
            [](const FOwnUser& UserRef)
            {
                // Connection established
            });
    }

    // https://getstream.io/chat/docs/react/init_and_users/?language=unreal#websocket-connections
    void WebsocketConnections()
    {
        Client->DisconnectUser();
    }
};
