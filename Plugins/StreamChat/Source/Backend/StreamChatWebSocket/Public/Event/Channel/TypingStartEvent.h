// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "TypingStartEvent.generated.h"

USTRUCT()
struct FTypingStartEvent : public FChannelEvent
{
    GENERATED_BODY()

    inline static FName StaticType = TEXT("typing.start");

    UPROPERTY()
    FString ParentId;

    UPROPERTY()
    FUserObjectDto User;
};
