// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "ChannelRequestDto.h"
#include "CoreMinimal.h"
#include "MessagePaginationParamsRequestDto.h"
#include "PaginationParamsRequestDto.h"

#include "ChannelGetOrCreateRequestDto.generated.h"

class FJsonObject;

/**
 * @brief #/components/schemas/ChannelGetOrCreateRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FChannelGetOrCreateRequestDto
{
    GENERATED_BODY()

    void SetMembers(const FPaginationParamsRequestDto& Value);

    void SetMessages(const FMessagePaginationParamsRequestDto& Value);

    void SetWatchers(const FPaginationParamsRequestDto& Value);

    UPROPERTY()
    FString ConnectionId;

    UPROPERTY()
    FChannelRequestDto Data;

    /// Start watching the channel
    UPROPERTY()
    bool bWatch;

    /// Refresh channel state
    UPROPERTY()
    bool bState;

    /// Fetch user presence info
    UPROPERTY()
    bool bPresence;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
