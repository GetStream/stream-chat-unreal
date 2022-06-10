// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"

namespace
{
UChatChannel* Channel = nullptr;

// https://getstream.io/chat/docs/unreal/channel_pagination/?language=unreal
void ChannelPagination()
{
    FMessagePaginationOptions MessagesPagination;
    MessagesPagination.Limit = 2;
    MessagesPagination.IdLte = TEXT("123");
    const FUserPaginationOptions MemberPagination{2, 0};
    const FUserPaginationOptions WatcherPagination{2, 0};
    Channel->Query(EChannelFlags::State, MessagesPagination, MemberPagination, WatcherPagination);
}
}    // namespace
