// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SearchWarningDto.generated.h"

/**
 * @brief #/components/schemas/SearchWarning
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FSearchWarningDto
{
    GENERATED_BODY()

    /// Description of the warning
    UPROPERTY()
    FString WarningDescription;

    /// Code corresponding to the warning
    UPROPERTY()
    int32 WarningCode = -1;

    /// Number of channels searched
    UPROPERTY()
    int32 ChannelSearchCount = -1;

    // Channel CIDs for the searched channels
    UPROPERTY()
    TArray<FString> ChannelSearchCids;
};
