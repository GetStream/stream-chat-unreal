// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Event/User/UserWatchingStartEvent.h"
#include "Event/User/UserWatchingStopEvent.h"
#include "StreamChatClientComponent.h"

namespace
{
UStreamChatClientComponent* Client = nullptr;
UChatChannel* Channel = nullptr;

// https://getstream.io/chat/docs/unreal/watch_channel/?language=unreal#to-start-watching-a-channel
void ToStartWatchingAChannel()
{
    const FChannelProperties Properties{
        TEXT("messaging"),    // Type
        TEXT("general"),      // Id
    };
    Client->WatchChannel(
        Properties,
        [](UChatChannel* Channel)
        {
            // Started watching channel
        });
}

// https://getstream.io/chat/docs/unreal/watch_channel/?language=unreal#watching-multiple-channels
void WatchingMultipleChannels()
{
    const FString UserId = TEXT("my-user-id");
    const FFilter Filter = FFilter::In(TEXT("members"), {UserId});
    const TArray<FChannelSortOption> SortOptions{{EChannelSortField::LastMessageAt, ESortDirection::Descending}};
    Client->QueryChannels(
        Filter,
        SortOptions,
        EChannelFlags::State | EChannelFlags::Watch,    // Flags
        {},                                             // Pagination Options
        [](const TArray<UChatChannel*> ReceivedChannels)
        {
            // Started watching channels
        });
}
// https://getstream.io/chat/docs/unreal/watch_channel/?language=unreal#stop-watching-a-channel
void StopWatchingAChannel()
{
    Channel->StopWatching();
}

// https://getstream.io/chat/docs/unreal/watch_channel/?language=unreal#watcher-count
void WatcherCount()
{
    const FChannelProperties Properties{
        TEXT("livestream"),            // Type
        TEXT("watch-this-channel"),    // Id
    };
    Client->WatchChannel(
        Properties,
        [](const UChatChannel* Channel)
        {
            // Do something with:
            Channel->State.WatcherCount;
        });
}

// https://getstream.io/chat/docs/unreal/watch_channel/?language=unreal#paginating-channel-watchers-with-channel.query
void PaginatingChannelWatchersWithChannelQuery()
{
    // Query the first 5 watchers of a channel
    const FUserPaginationOptions WatcherPagination{
        5,    // Limit
        0,    // Offset
    };
    Channel->Query(
        EChannelFlags::State,    // Flags
        {},                      // Message Pagination
        {},                      // Member Pagination
        WatcherPagination,
        []
        {
            // Do something with:
            Channel->State.Watchers;
        });
}

// https://getstream.io/chat/docs/unreal/watch_channel/?language=unreal#listening-to-changes-in-watchers
void ListeningToChangesInWatchers()
{
    Channel->On<FUserWatchingStartEvent>(
        [](const FUserWatchingStartEvent& Event)
        {
            // Do something with:
            Event.User;
        });

    Channel->On<FUserWatchingStopEvent>(
        [](const FUserWatchingStopEvent& Event)
        {
            // Do something with:
            Event.User;
        });
}

}    // namespace
