// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"

#include "SendEventRequest.generated.h"

/**
 *
 */
USTRUCT()
struct FSendEventRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    FJsonObjectWrapper Event;
};
