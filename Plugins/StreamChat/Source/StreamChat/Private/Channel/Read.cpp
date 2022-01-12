// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Read.h"

#include "Response/Channel/ReadDto.h"
#include "User/UserManager.h"

FRead::FRead() = default;

FRead::FRead(UUserManager& UserManager, const FReadDto& Dto)
    : User{UserManager.UpsertUser(Dto.User)}, UnreadMessages{static_cast<int32>(Dto.UnreadMessages)}, LastRead{Dto.LastRead}
{
}
