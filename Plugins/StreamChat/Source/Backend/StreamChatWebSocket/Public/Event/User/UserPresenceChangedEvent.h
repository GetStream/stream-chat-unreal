// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChatEvent.h"
#include "UserObjectDto.h"

#include "UserPresenceChangedEvent.generated.h"

/**
 * #/components/schemas/UserPresenceChangedEvent
 */
USTRUCT()
struct FUserPresenceChangedEvent : public FChatEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("user.presence.changed");

    UPROPERTY()
    FUserObjectDto User;
};
