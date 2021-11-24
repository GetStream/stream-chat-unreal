// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/MessageEvent.h"

#include "MessageUpdatedEvent.generated.h"

/**
 * @brief #/components/schemas/MessageUpdatedEvent
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMessageUpdatedEvent : public FMessageEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.updated");

    // TODO everything else
};
