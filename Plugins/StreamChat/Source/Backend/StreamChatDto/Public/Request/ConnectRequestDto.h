// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Device/DeviceFieldsDto.h"
#include "UserObjectDto.h"

#include "ConnectRequestDto.generated.h"

/**
 * @brief #/components/schemas/ConnectRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FConnectRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    FUserObjectDto UserDetails;

    UPROPERTY()
    FDeviceFieldsDto Device;
};
