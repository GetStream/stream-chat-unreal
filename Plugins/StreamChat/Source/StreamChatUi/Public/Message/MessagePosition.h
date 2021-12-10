// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MessagePosition.generated.h"

UENUM(BlueprintType)
enum class EMessagePosition : uint8
{
    Opening,
    End
};
