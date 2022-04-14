// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"

#include "UpdateChannelPartialRequestDto.generated.h"

/**
 * @brief #/components/schemas/UpdateChannelPartialRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateChannelPartialRequestDto
{
    GENERATED_BODY()

    /// Sets new field values
    UPROPERTY()
    FJsonObjectWrapper Set;

    /// Array of field names to unset
    UPROPERTY()
    TArray<FString> Unset;
};
