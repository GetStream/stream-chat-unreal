// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Member.h"

#include "Response/Channel/ChannelMemberDto.h"
#include "User/UserManager.h"

FMember::FMember() = default;

FMember::FMember(const FUserRef& User) : User{User}
{
}

FMember::FMember(const FChannelMemberDto& Dto, UUserManager* UserManager) : User{UserManager->UpsertUser(Dto.User)}
{
}
