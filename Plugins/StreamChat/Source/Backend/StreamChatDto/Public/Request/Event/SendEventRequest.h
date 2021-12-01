// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"

#include "SendEventRequest.generated.h"

/**
 * @brief #/components/schemas/SendEventRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FSendEventRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    FJsonObjectWrapper Event;
};
