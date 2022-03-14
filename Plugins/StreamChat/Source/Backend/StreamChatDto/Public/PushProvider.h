// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PushProvider.generated.h"

UENUM(BlueprintType)
enum class EPushProvider : uint8
{
    Firebase,
    Apn,
    Huawei
};
