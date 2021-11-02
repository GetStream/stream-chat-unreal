#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "Response/Message/MessageDto.h"

#include "MessageEvent.generated.h"

USTRUCT()
struct FMessageEvent : public FChannelEvent
{
    GENERATED_BODY()

    STREAMCHATWEBSOCKET_API static void DeserializeExtra(const FJsonObject&, FMessageEvent&);

    UPROPERTY()
    FMessageDto Message;
};
