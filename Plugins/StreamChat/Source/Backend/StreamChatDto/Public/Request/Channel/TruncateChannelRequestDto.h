// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Request/Message/MessageRequestDto.h"

#include "TruncateChannelRequestDto.generated.h"

/**
 * @brief #/components/schemas/TruncateChannelRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FTruncateChannelRequestDto
{
    GENERATED_BODY()

    void SetMessage(const FMessageRequestDto& Message);

    /// Permanently delete channel data (messages, reactions, etc.)
    UPROPERTY()
    bool bHardDelete = false;

    /// When `message` is set disables all push notifications for it
    UPROPERTY()
    bool bSkipPush = false;

    /// Truncate channel data up to `truncated_at`. The system message (if provided) creation time is always greater than `truncated_at`
    UPROPERTY()
    FDateTime TruncatedAt = FDateTime{0};

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
