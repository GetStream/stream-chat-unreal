// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Read.h"

#include "Response/Channel/ReadDto.h"
#include "User/UserManager.h"

FRead::FRead() = default;

FRead::FRead(const FReadDto& Dto, UUserManager* UserManager)
    : User{UserManager->UpsertUser(Dto.User)}, UnreadMessages{static_cast<int32>(Dto.UnreadMessages)}, LastRead{Dto.LastRead}
{
}

void FRead::AddUnreaMessage()
{
    UnreadMessages++;
}

void FRead::MarkMessageRead()
{
    ensure(UnreadMessages > 0);
    UnreadMessages--;
    LastRead = FDateTime::UtcNow();
}
