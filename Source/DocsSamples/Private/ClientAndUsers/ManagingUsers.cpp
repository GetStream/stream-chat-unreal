// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"
#include "User/User.h"
namespace ManagingUsers
{
UStreamChatClientComponent* Client = nullptr;
FUser User;

// https://getstream.io/chat/docs/unreal/authless_users/?language=unreal#guest-users
void ServerSideUserUpdatesBatch()
{
    Client->UpsertUsers({User});
}

}    // namespace ManagingUsers
