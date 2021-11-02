#pragma once

#include "CoreMinimal.h"

#include "MessageSide.generated.h"

UENUM(BlueprintType)

enum class EMessageSide : uint8
{
    // Left
    You,
    // Right
    Me
};
