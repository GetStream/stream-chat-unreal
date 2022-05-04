// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Moderation/MutedChannel.h"

#include "User/UserManager.h"

FMutedChannel::FMutedChannel() = default;

FMutedChannel::FMutedChannel(const FChannelMuteDto& Dto, UUserManager* UserManager)
    : CreatedAt{Dto.CreatedAt}, UpdatedAt{Dto.UpdatedAt}, Expires{Dto.Expires}, Cid{Dto.Channel.Cid}, User{UserManager->UpsertUser(Dto.User)}
{
}
