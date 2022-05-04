// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MuteUserRequestDto.generated.h"

/**
 * @brief #/components/schemas/MuteUserRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FMuteUserRequestDto
{
    GENERATED_BODY()

    /// User IDs to mute (if multiple users)
    UPROPERTY() TArray<FString> TargetIds;

    /// Duration of mute in minutes
    UPROPERTY()
    float Timeout = TNumericLimits<float>::Max();
};
