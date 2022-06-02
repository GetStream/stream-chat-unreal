// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/OwnUser.h"

#include "OwnUserDto.h"
#include "User/UserManager.h"
#include "Util.h"

FOwnUser::FOwnUser() = default;

FOwnUser::FOwnUser(const FUserRef& InUser) : User{InUser}
{
}

FOwnUser::FOwnUser(const FOwnUserDto& Dto, UUserManager* UserManager)
    : User(UserManager->UpsertUser(Dto))
    , UnreadCount(Dto.UnreadCount)
    , TotalUnreadCount(Dto.TotalUnreadCount)
    , UnreadChannels(Dto.UnreadChannels)
    , MutedUsers{Util::Convert<FMutedUser>(Dto.Mutes, UserManager)}
    , MutedChannels{Util::Convert<FMutedChannel>(Dto.ChannelMutes, UserManager)}
{
}

void FOwnUser::Update(const FOwnUser& OwnUser)
{
    UnreadCount = OwnUser.UnreadCount;
    TotalUnreadCount = OwnUser.TotalUnreadCount;
    UnreadChannels = OwnUser.UnreadChannels;
    MutedUsers = OwnUser.MutedUsers;
    MutedChannels = OwnUser.MutedChannels;
}

bool FOwnUser::HasMutedUser(const FUserRef& TargetUser) const
{
    return MutedUsers.ContainsByPredicate([&TargetUser](const FMutedUser& M) { return M.Target == TargetUser; });
}
