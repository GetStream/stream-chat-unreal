// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"
#include "User/User.h"
namespace
{
UStreamChatClientComponent* Client = nullptr;

// https://getstream.io/chat/docs/unreal/query_users/?language=unreal
void QueryUsers1()
{
    Client->QueryUsers(
        FFilter::In(TEXT("id"), {TEXT("john"), TEXT("jack"), TEXT("jessie")}),
        {},      // Sort
        true,    // Presence
        {},      // Limit
        {},      // Offset
        [](const TArray<FUserRef>& Users)
        {
            // Do something with Users
        });
}

void QueryUsers2()
{
    Client->QueryUsers(
        FFilter::In(TEXT("id"), {TEXT("john"), TEXT("jack"), TEXT("jessie")}),
        {FUserSortOption{EUserSortField::LastActive}},
        true,    // bPresence
        20,      // Limit
        0,       // Offset
        [](const TArray<FUserRef>& Users)
        {
            // Do something with Users
        });
}

void QueryBannedUsers()
{
    Client->QueryUsers(
        FFilter::And({
            FFilter::Equal(TEXT("id"), TEXT("jessie")),
            FFilter::Equal(TEXT("banned"), true),
        }),
        {FUserSortOption{EUserSortField::LastActive}},
        true,    // Presence
        {},      // Limit
        {},      // Offset
        [](const TArray<FUserRef>& Users)
        {
            // Do something with Users
        });
}

// https://getstream.io/chat/docs/unreal/query_users/?language=unreal#1.-by-name
void AutocompleteByName()
{
    Client->QueryUsers(
        FFilter::Autocomplete(TEXT("name"), TEXT("ro")),
        {},      // Sort
        true,    // Presence
        {},      // Limit
        {},      // Offset
        [](const TArray<FUserRef>& Users)
        {
            // Do something with Users
        });
}

// https://getstream.io/chat/docs/unreal/query_users/?language=unreal#2.-by-id
void AutocompleteById()
{
    Client->QueryUsers(
        FFilter::Autocomplete(TEXT("id"), TEXT("userid")),
        {},      // Sort
        true,    // Presence
        {},      // Limit
        {},      // Offset
        [](const TArray<FUserRef>& Users)
        {
            // Do something with Users
        });
}

}    // namespace
