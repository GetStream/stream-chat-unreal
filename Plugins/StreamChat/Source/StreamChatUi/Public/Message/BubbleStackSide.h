#pragma once

#include "CoreMinimal.h"

#include "BubbleStackSide.generated.h"

UENUM(BlueprintType)

enum class EBubbleStackSide : uint8
{
    // Left
    You,
    // Right
    Me
};
