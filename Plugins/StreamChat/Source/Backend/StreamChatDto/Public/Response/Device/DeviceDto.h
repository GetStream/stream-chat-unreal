// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PushProvider.h"

#include "DeviceDto.generated.h"

/**
 * @brief #/components/schemas/Response
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FDeviceDto
{
    GENERATED_BODY()

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    /// Whether device is disabled on not
    UPROPERTY()
    bool bDisabled = false;

    /// Reason explaining why device had been disabled
    UPROPERTY()
    FString DisabledReason;

    /// Device ID
    UPROPERTY()
    FString Id;

    /// Push provider
    UPROPERTY()
    EPushProvider PushProvider;

    UPROPERTY()
    FString UserId;
};
