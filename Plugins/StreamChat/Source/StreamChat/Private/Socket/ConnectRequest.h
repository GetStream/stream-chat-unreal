// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DeviceFields.h"
#include "User.h"

#include "ConnectRequest.generated.h"

// ReSharper disable CppUE4CodingStandardNamingViolationWarning

/**
 *
 */
USTRUCT()
struct FConnectRequest
{
	GENERATED_BODY()

	UPROPERTY()
	bool server_determines_connection_id;

	UPROPERTY()
	FString client_id;

	UPROPERTY()
	FUser user_details;

	UPROPERTY()
	FDeviceFields device;
};
