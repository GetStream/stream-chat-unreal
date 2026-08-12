// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Moderation/UserBlock.h"

#include "User/UserManager.h"

FUserBlock::FUserBlock(const FBlockedUserDto& Dto, UUserManager* UserManager)
    : BlockedUser{UserManager->UpsertUser(Dto.BlockedUser)}, BlockedBy{UserManager->UpsertUser(Dto.User)}, CreatedAt{Dto.CreatedAt}
{
}
