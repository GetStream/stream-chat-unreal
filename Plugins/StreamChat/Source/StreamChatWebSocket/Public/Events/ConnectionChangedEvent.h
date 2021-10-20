#pragma once

#include "CoreMinimal.h"
#include "LocalEvent.h"

#include "ConnectionChangedEvent.generated.h"

/**
 * When the state of the connection changed
 */
USTRUCT()
struct FConnectionChangedEvent : public FLocalEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("connection.changed");

    UPROPERTY()
    bool bOnline;
};
