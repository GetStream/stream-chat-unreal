// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Event/User/UserPresenceChangedEvent.h"
#include "StreamChatClientComponent.h"
#include "User/User.h"

namespace
{
UStreamChatClientComponent* Client;

// https://getstream.io/chat/docs/unreal/presence_format/?language=unreal#invisible
void Invisible()
{
    FUser User{TEXT("user-id")};
    User.bInvisible = true;
    const FString Token{TEXT("{{ chat_user_token }}")};
    Client->ConnectUser(User, Token);
}

// https://getstream.io/chat/docs/unreal/presence_format/?language=unreal#listening-to-presence-changes
void ListeningToPresenceChanges()
{
    // You need to be watching some channels/queries to be able to get presence events.
    // Here are three different ways of doing that:

    // 1. Watch a single channel with presence = true set
    constexpr EChannelFlags AllFlags = EChannelFlags::Watch | EChannelFlags::State | EChannelFlags::Presence;
    const FChannelProperties Properties{TEXT("messaging"), TEXT("unrealdevs")};
    Client->QueryChannel(Properties, AllFlags);

    // 2. Query some channels with 'presence' set
    Client->QueryChannels(
        FFilter::Equal(TEXT("type"), TEXT("messaging")),    // Channel filter
        {},                                                 // Sort options
        AllFlags                                            // Flags
    );

    // 3. Query some users with presence = true set
    Client->QueryUsers(
        FFilter::In(TEXT("id"), {TEXT("john"), TEXT("jack")}),
        {},         // Sort options
        true,       // Presence
        {{0, 2}}    // Pagination options
    );

    // Finally, subscribe to presence to events
    Client->On<FUserPresenceChangedEvent>(
        [](const FUserPresenceChangedEvent& Event)
        {
            // Handle change
        });
}
}    // namespace
