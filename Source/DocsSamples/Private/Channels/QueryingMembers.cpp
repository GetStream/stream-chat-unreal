// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "User/User.h"

namespace
{
UChatChannel* Channel = nullptr;
FUserRef LastMember;

// https://getstream.io/chat/docs/unreal/query_members/?language=unreal#pagination-and-ordering
void PaginationAndOrdering()
{
    // returns up to 100 members ordered by created_at descending
    {
        const TArray<FUserSortOption> Sort{{EUserSortField::CreatedAt, ESortDirection::Descending}};
        Channel->QueryMembers({}, Sort, {});
    }

    // returns up to 100 members ordered by user_id descending
    {
        const TArray<FUserSortOption> Sort{{EUserSortField::Id, ESortDirection::Descending}};
        Channel->QueryMembers({}, Sort, {});
    }

    // paginate by user_id in ascending order
    {
        const TArray<FUserSortOption> Sort{{EUserSortField::Id, ESortDirection::Ascending}};
        FMessagePaginationOptions Options{};
        Options.IdLt = LastMember->Id;
        Channel->QueryMembers({}, Sort, Options);
    }

    // paginate by created at in ascending order
    {
        const TArray<FUserSortOption> Sort{{EUserSortField::CreatedAt, ESortDirection::Ascending}};
        FMessagePaginationOptions Options{};
        Options.CreatedAtBefore = LastMember->CreatedAt;
        Channel->QueryMembers({}, Sort, Options);
    }

    // paginate using offset
    {
        const FMessagePaginationOptions Options{
            {},    // Limit
            20,    // Offset
        };
        Channel->QueryMembers({}, {}, Options);
    }
}

void Filters()
{
    // query members by user.name
    Channel->QueryMembers(FFilter::Equal(TEXT("name"), TEXT("tommaso")));

    // autocomplete members by user name
    Channel->QueryMembers(FFilter::Autocomplete(TEXT("name"), TEXT("tomm")));

    // query member by id
    Channel->QueryMembers(FFilter::Equal(TEXT("id"), TEXT("tommaso")));

    // query multiple members by id
    Channel->QueryMembers(FFilter::In(TEXT("id"), {TEXT("tommaso"), TEXT("thierry")}));

    // query channel moderators
    Channel->QueryMembers(FFilter::Equal(TEXT("is_moderator"), true));

    // query for banned members in channel
    Channel->QueryMembers(FFilter::Equal(TEXT("banned"), true));

    // query members with pending invites
    Channel->QueryMembers(FFilter::Equal(TEXT("invite"), TEXT("pending")));

    // query all the members
    Channel->QueryMembers();

    // paginate channel members
    Channel->QueryMembers({}, {}, {10, 10});

    // order results by member created at descending
    Channel->QueryMembers({}, {{EUserSortField::CreatedAt, ESortDirection::Descending}});

    // query for user.email (currently the only supported custom field)
    Channel->QueryMembers(FFilter::Equal(TEXT("email"), TEXT("marcelo@getstream.io")));
}
}    // namespace
