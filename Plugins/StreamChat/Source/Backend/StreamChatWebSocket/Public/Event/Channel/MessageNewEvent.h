// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "MessageNewEvent.generated.h"

/**
 * @brief #/components/schemas/MessageNewEvent
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMessageNewEvent : public FMessageEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.new");

    // TODO everything else
};
