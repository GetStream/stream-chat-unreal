// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DeviceFields.h"
#include "User.h"

#include "ConnectRequest.generated.h"

/**
 *
 */
USTRUCT()
struct FConnectRequest
{
	GENERATED_BODY()

	UPROPERTY()
	bool bServerDeterminesConnectionId;

	UPROPERTY()
	FString ClientId;

	UPROPERTY()
	FUser UserDetails;

	UPROPERTY()
	FDeviceFields Device;
};
