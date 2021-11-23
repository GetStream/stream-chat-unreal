// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "MessageDeletedEvent.generated.h"

/**
 * #/components/schemas/MessageDeletedEvent
 */
USTRUCT()
struct FMessageDeletedEvent : public FMessageEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.deleted");

    UPROPERTY()
    bool bHardDelete;

    // TODO everything else
};
