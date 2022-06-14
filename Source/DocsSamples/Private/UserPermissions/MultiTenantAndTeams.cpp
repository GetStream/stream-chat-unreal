// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

namespace MultiTenantAndTeams
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/multi_tenant_chat/?language=unreal#channel-team
void ChannelTeam()
{
    const FChannelProperties Properties{
        TEXT("messaging"),      // Type
        TEXT("red-general"),    // Id
        TEXT("red"),            // Team

    };
    Client->CreateChannel(
        Properties,
        [](UChatChannel* Channel)
        {
            // Channel created
        });
}

// https://getstream.io/chat/docs/unreal/multi_tenant_chat/?language=unreal#user-search
void UserSearch()
{
    // search for users with name Jordan that are part of the same team as the authorized user
    Client->QueryUsers(FFilter::Equal(TEXT("name"), TEXT("Jordan")));

    // search for users with name Nick in all teams
    Client->QueryUsers(FFilter::And({
        FFilter::Equal(TEXT("name"), TEXT("Nick")),
        FFilter::Empty(TEXT("teams")),
    }));

    // search for users with name Dan in subset of teams
    Client->QueryUsers(FFilter::And({
        FFilter::Equal(TEXT("name"), TEXT("Nick")),
        FFilter::In(TEXT("teams"), {TEXT("red"), TEXT("blue")}),
    }));

    // search for users with name Tom that don't have any team assigned
    Client->QueryUsers(FFilter::And({
        FFilter::Equal(TEXT("name"), TEXT("Tom")),
        FFilter::Equal(TEXT("teams"), nullptr),
    }));
}

// https://getstream.io/chat/docs/unreal/multi_tenant_chat/?language=unreal#query-channels
void QueryChannels()
{
    // query all channels from teams that user is a part of
    Client->QueryChannels();

    // query all channels from all teams
    Client->QueryChannels(FFilter::Empty(TEXT("team")));
}

}    // namespace MultiTenantAndTeams
