// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BlockUserRequestDto.generated.h"

/**
 * @brief #/components/schemas/BlockUsersRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FBlockUserRequestDto
{
    GENERATED_BODY()

    /// Id of the user to block or unblock
    UPROPERTY()
    FString BlockedUserId;
};
