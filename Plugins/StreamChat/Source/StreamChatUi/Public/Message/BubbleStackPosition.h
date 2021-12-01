// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BubbleStackPosition.generated.h"

UENUM(BlueprintType)
enum class EBubbleStackPosition : uint8
{
    Opening,
    End
};
