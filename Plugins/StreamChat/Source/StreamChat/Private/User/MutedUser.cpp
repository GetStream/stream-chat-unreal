// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "User/MutedUser.h"

#include "UserManager.h"

FMutedUser::FMutedUser() = default;

FMutedUser::FMutedUser(const FUserMuteDto& Dto, UUserManager* UserManager) : CreatedAt{Dto.CreatedAt}, UpdatedAt{Dto.UpdatedAt}, Expires{Dto.Expires}
// , Target{UserManager.UpsertUser(Dto.Target)}
// , User{UserManager.UpsertUser(Dto.User)}
{
}
