// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PushProvider.h"

#include "DeviceFields.generated.h"

/**
 *
 */
USTRUCT()
struct FDeviceFields
{
	GENERATED_BODY()

	UPROPERTY()
	EPushProvider PushProvider;

	UPROPERTY()
	FString Id;
};
