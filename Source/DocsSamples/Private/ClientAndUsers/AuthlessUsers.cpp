// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"
#include "User/User.h"
namespace AuthlessUsers
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/authless_users/?language=unreal#guest-users
void GuestUsers()
{
    const FUser User{TEXT("tommaso")};
    Client->ConnectGuestUser(
        User,
        [](const FOwnUser& Guest)
        {
            // Guest connected
        });
}

// https://getstream.io/chat/docs/unreal/authless_users/?language=unreal#anonymous-users
void AnonymousUsers()
{
    Client->ConnectAnonymousUser(
        [](const FOwnUser& User)
        {
            // Connected
        });
}

}    // namespace AuthlessUsers
