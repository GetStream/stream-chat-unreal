#pragma once

#include "Api/Dto/Response/MessageDto.h"
#include "ChatEvent.h"
#include "CoreMinimal.h"

#include "NewMessageEvent.generated.h"

/**
 * Represents an event that happened in Stream Chat
 */
USTRUCT()
struct FNewMessageEvent : public FChatEvent
{
    GENERATED_BODY()

    UPROPERTY()
    FString ConnectionId;

    UPROPERTY()
    FMessageDto Message;

    // TODO everything else
};
