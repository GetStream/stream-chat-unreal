// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PushProvider.h"

#include "DeviceFieldsDto.generated.h"

/**
 * @brief #/components/schemas/DeviceFields
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FDeviceFieldsDto
{
    GENERATED_BODY()

    UPROPERTY()
    EPushProvider PushProvider = EPushProvider::Firebase;

    UPROPERTY()
    FString Id;
};
