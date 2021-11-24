// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DeviceFieldsDto.h"
#include "UserObjectDto.h"

#include "UserObjectRequestDto.generated.h"

/**
 * @brief #/components/schemas/UserObjectRequest
 * @ingroup StreamChatDto
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
    FDeviceFieldsDto Device;
};
