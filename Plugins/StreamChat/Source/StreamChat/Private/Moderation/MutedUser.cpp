// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Moderation/MutedUser.h"

#include "User/UserManager.h"

FMutedUser::FMutedUser() = default;

FMutedUser::FMutedUser(const FUserMuteDto& Dto, UUserManager* UserManager)
    : CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
    , Expires{Dto.Expires}
    , Target{UserManager->UpsertUser(Dto.Target)}
    , User{UserManager->UpsertUser(Dto.User)}
{
}
