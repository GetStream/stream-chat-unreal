// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DeviceDto.h"
#include "Response/ResponseDto.h"

#include "ListDevicesResponseDto.generated.h"

/**
 * @brief #/components/schemas/Response
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FListDevicesResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// List of devices
    UPROPERTY()
    TArray<FDeviceDto> Devices;
};
