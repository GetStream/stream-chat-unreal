// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelSortOption.h"
#include "Channel/Filter.h"
#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

namespace
{
UStreamChatClientComponent* Client = nullptr;
FUserRef User;

// https://getstream.io/chat/docs/unreal/query_channels/?language=unreal
void QueryListOfChannels()
{
    const FFilter Filter = FFilter::And({
        FFilter::In(TEXT("members"), {TEXT("thierry")}),
        FFilter::Equal(TEXT("type"), TEXT("messaging")),
    });
    const TArray<FChannelSortOption> SortOptions{{EChannelSortField::LastMessageAt, ESortDirection::Descending}};
    Client->QueryChannels(
        Filter,
        SortOptions,
        EChannelFlags::State,
        {},    // Pagination options
        [](const TArray<UChatChannel*> ReceivedChannels)
        {
            // Do something with ReceivedChannels
        });
}

// https://getstream.io/chat/docs/unreal/query_channels/?language=unreal#messaging-and-team
void MessagingAndTeam()
{
    const FFilter Filter = FFilter::And({
        FFilter::In(TEXT("members"), {TEXT("thierry")}),
        FFilter::Equal(TEXT("type"), TEXT("messaging")),
    });
}

// https://getstream.io/chat/docs/unreal/query_channels/?language=unreal#support
void Support()
{
    const FFilter Filter = FFilter::And({
        FFilter::Equal(TEXT("agent_id"), User->Id),
        FFilter::In(TEXT("status"), {TEXT("pending"), TEXT("open"), TEXT("new")}),
    });
}
// https://getstream.io/chat/docs/unreal/query_channels/?language=unreal#pagination
void Pagination()
{
    const FFilter Filter = FFilter::In(TEXT("members"), {TEXT("thierry")});
    const TArray<FChannelSortOption> SortOptions{
        {EChannelSortField::LastMessageAt, ESortDirection::Descending},
    };
    const FPaginationOptions PaginationOptions{
        20,    // Limit
        10,    // Offset
    };
    Client->QueryChannels(
        Filter,
        SortOptions,
        EChannelFlags::State,
        PaginationOptions,
        [](const TArray<UChatChannel*> ReceivedChannels)
        {
            // Started watching channels
        });
}
}    // namespace
