// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"

#include "UpdateUserPartialRequestDto.generated.h"

/**
 * @brief #/components/schemas/UpdateUserPartialRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateUserPartialRequestDto
{
    GENERATED_BODY()

    /// ID of user to update
    UPROPERTY()
    FString Id;

    /// Sets new field values
    UPROPERTY()
    FJsonObjectWrapper Set;

    /// Array of field names to unset
    UPROPERTY()
    TArray<FString> Unset;
};
