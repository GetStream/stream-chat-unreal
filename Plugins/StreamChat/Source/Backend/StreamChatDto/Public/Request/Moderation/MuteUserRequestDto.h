// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
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

    /// Duration of mute in minutes
    void SetTimeout(float);

    /// User IDs to mute (if multiple users)
    UPROPERTY()
    TArray<FString> TargetIds;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
