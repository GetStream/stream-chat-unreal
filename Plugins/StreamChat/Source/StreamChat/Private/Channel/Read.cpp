// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Read.h"

#include "Response/Channel/ReadDto.h"
#include "User/UserManager.h"

FRead::FRead() = default;

FRead::FRead(const FUserRef& InUser, const int32 InUnreadMessages, const FDateTime& InLastRead)
    : User{InUser}, UnreadMessages{InUnreadMessages}, LastRead{InLastRead}
{
}

FRead::FRead(const FReadDto& Dto, UUserManager* UserManager)
    : User{UserManager->UpsertUser(Dto.User)}, UnreadMessages{static_cast<int32>(Dto.UnreadMessages)}, LastRead{Dto.LastRead}
{
}

void FRead::AddUnreadMessage()
{
    UnreadMessages++;
}

void FRead::MarkRead()
{
    ensure(UnreadMessages > 0);
    UnreadMessages = 0;
    LastRead = FDateTime::UtcNow();
}
