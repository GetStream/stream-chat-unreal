// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DeviceFields.h"
#include "UserObjectDto.h"

#include "UserObjectRequestDto.generated.h"

/**
 *
 */
USTRUCT()
struct FUserObjectRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    bool bServerDeterminesConnectionId;

    UPROPERTY()
    FString ClientId;

    UPROPERTY()
    FUserObjectDto UserDetails;

    UPROPERTY()
    FDeviceFields Device;
};
