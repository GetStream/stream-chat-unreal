// Copyright 2022 Stream.IO, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "CustomEvents.generated.h"

// Custom events must be defined as a USTRUCT with a static FName member `StaticType`:
USTRUCT()
struct FMyCustomEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("my_custom_event")};
        return Type;
    }

    // Custom members must be UPROPERTYs to be picked up by the serializer
    UPROPERTY()
    float MyCustomData;
};
