// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Moderation/Ban.h"
#include "StreamChatClientComponent.h"

namespace ModerationTools
{
UStreamChatClientComponent* Client;
UChatChannel* Channel;
FMessage Message;
FUserRef User;

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#flag
void Flag()
{
    Client->FlagMessage(Message);
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#mutes
void Mutes()
{
    Client->MuteUser(User);

    Client->UnmuteUser(User);
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#ban
void Ban()
{
    // ban a user for 60 minutes from all channel
    Client->BanUser(User, FTimespan::FromMinutes(60.), {TEXT("Banned for one hour")});

    // ban a user and their IP address for 24 hours
    Client->BanUser(User, FTimespan::FromHours(24.), {TEXT("Please come back tomorrow")}, true);

    // ban a user from a specific channel
    Channel->BanMember(User, {}, {TEXT("Profanity is not allowed here")});

    // remove ban from channel
    Channel->UnbanMember(User);

    // remove global ban
    Client->UnbanUser(User);
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#query-banned-users
void QueryBannedUsers()
{
    // Retrieve the list of banned users
    Client->QueryUsers(
        FFilter::Equal(TEXT("banned"), true),
        {},      // Sort options
        true,    // Presence
        {},      // Pagination options
        [](const TArray<FUserRef> Users)
        {
            // Do something with Users
        });

    // Query for banned members from one channel
    Client->QueryBannedUsers(
        FFilter::Equal(TEXT("channel_cid"), TEXT("ChannelType:ChannelId")),
        {},    // Sort options
        {},    // Pagination options
        [](const TArray<FBan> Bans)
        {
            // Do something with Bans
        });
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#query-bans-endpoint
void QueryBansEndpoint()
{
    // Get the bans for channel livestream:123 in reverse chronological order of banning
    Client->QueryBannedUsers(
        FFilter::Equal(TEXT("channel_cid"), TEXT("livestream:123")),
        {{EBanSortField::CreatedAt, ESortDirection::Descending}},
        {},
        [](const TArray<FBan> Bans)
        {
            // Do something with Bans
        });

    // Get the page of bans which were created before or equal to the current time for the same channel
    FBanPaginationOptions PaginationOptions;
    PaginationOptions.CreatedAtBeforeOrEqual = FDateTime::UtcNow();
    Client->QueryBannedUsers(
        FFilter::Equal(TEXT("channel_cid"), TEXT("livestream:123")),
        {{EBanSortField::CreatedAt, ESortDirection::Descending}},
        {PaginationOptions},
        [](const TArray<FBan> Bans)
        {
            // Do something with Bans
        });
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#shadow-ban
void ShadowBan()
{
    // shadow ban a user from all channels
    Client->ShadowBanUser(User);

    // shadow ban a user from a channel
    Channel->ShadowBanMember(User);

    // remove shadow ban from channel
    Channel->ShadowUnbanMember(User);

    // remove global shadow ban
    Client->ShadowUnbanUser(User);
}
}    // namespace ModerationTools
