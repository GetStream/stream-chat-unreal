#pragma once

#include "ChatEvent.h"
#include "CoreMinimal.h"
#include "Response/MessageDto.h"

#include "NewMessageEvent.generated.h"

/**
 * Represents an event that happened in Stream Chat
 */
USTRUCT()
struct FNewMessageEvent : public FChatEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("message.new");

    UPROPERTY()
    FMessageDto Message;

    // TODO everything else
};
