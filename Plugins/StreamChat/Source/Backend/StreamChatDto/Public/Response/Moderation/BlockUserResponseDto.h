// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BlockUserResponseDto.generated.h"

/**
 * @brief #/components/schemas/BlockUsersResponse
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FBlockUserResponseDto
{
    GENERATED_BODY()

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    /// Id of the user who created the block
    UPROPERTY()
    FString BlockedByUserId;

    /// Id of the user who was blocked
    UPROPERTY()
    FString BlockedUserId;

    /// When the block was created
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};
};
