// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/Device/DeviceDto.h"
#include "UserRef.h"

#include "Device.generated.h"

/**
 * @brief A push notification device
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FDevice
{
    GENERATED_BODY()
    FDevice() = default;
    explicit FDevice(const FDeviceDto&, UUserManager*);

    /// Date/time of creation
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Device")
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Device")
    FDateTime UpdatedAt = FDateTime{0};

    /// Whether device is disabled on not
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Device")
    bool bDisabled = false;

    /// Reason explaining why device had been disabled
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Device")
    FString DisabledReason;

    /// Device ID
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Device")
    FString Id;

    /// Push provider
    UPROPERTY(BlueprintReadOnly, Category = "Stream|Device")
    EPushProvider PushProvider = EPushProvider::Firebase;

    UPROPERTY(BlueprintReadOnly, Category = "Stream|Device")
    FUserRef User;
};
