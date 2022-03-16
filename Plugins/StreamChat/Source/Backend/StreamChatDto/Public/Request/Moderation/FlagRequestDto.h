// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FlagRequestDto.generated.h"

/**
 * @brief #/components/schemas/FlagRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/#flag
 */
USTRUCT()
struct STREAMCHATDTO_API FFlagRequestDto
{
    GENERATED_BODY()

    /// ID of the message when reporting a message
    UPROPERTY()
    FString TargetMessageId;

    /// ID of the user when reporting a user
    UPROPERTY()
    FString TargetUserId;
};
