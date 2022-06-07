// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Member.h"

#include "Response/Channel/ChannelMemberDto.h"
#include "User/UserManager.h"

FMember::FMember() = default;

FMember::FMember(const FUserRef& User) : User{User}
{
}

FMember::FMember(const FChannelMemberDto& Dto, UUserManager* UserManager)
    : User{UserManager->UpsertUser(Dto.User)}, BanExpires{Dto.BanExpires}, bBanned{Dto.bBanned}, ChannelRole{Dto.ChannelRole}
{
}

bool FMember::operator==(const FUserRef& Other) const
{
    return this->User == Other;
}

bool FMember::operator==(const FMember& Other) const
{
    return this->User == Other.User;
}

uint32 GetTypeHash(const FMember& M)
{
    return GetTypeHash(M.User);
}
