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

    void SetDevice(const FDeviceFieldsDto& Device)
    {
        AdditionalFields.Set(TEXT("device"), Device);
    }

    UPROPERTY()
    FUserObjectDto UserDetails;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
