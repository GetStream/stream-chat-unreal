// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/Device.h"

#include "User/UserManager.h"

FDevice::FDevice(const FDeviceDto& Dto, UUserManager* UserManager)
    : CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
    , bDisabled{Dto.bDisabled}
    , DisabledReason{Dto.DisabledReason}
    , Id{Dto.Id}
    , PushProvider{Dto.PushProvider}
    , User{UserManager->UpsertUser(Dto.UserId)}
{
}
