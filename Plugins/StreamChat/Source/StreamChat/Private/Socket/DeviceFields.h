// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PushProvider.h"

#include "DeviceFields.generated.h"

// ReSharper disable CppUE4CodingStandardNamingViolationWarning

/**
 *
 */
USTRUCT()
struct FDeviceFields
{
	GENERATED_BODY()

	UPROPERTY()
	EPushProvider push_provider;

	UPROPERTY()
	FString id;
};
