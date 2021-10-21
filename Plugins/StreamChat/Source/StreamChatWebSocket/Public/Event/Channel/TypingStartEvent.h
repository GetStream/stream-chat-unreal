#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "TypingStartEvent.generated.h"

USTRUCT()
struct FTypingStartEvent : public FChannelEvent
{
    GENERATED_BODY()

    UPROPERTY()
    FString ParentId;

    UPROPERTY()
    FUserObjectDto User;
};
