// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"

#include "ChannelRequestDto.generated.h"

/**
 * @brief #/components/schemas/ChannelRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FChannelRequestDto
{
    GENERATED_BODY()

    // Enable or disable auto translation
    UPROPERTY()
    bool bAutoTranslationEnabled = false;

    UPROPERTY()
    FString AutoTranslationLanguage;

    // TODO ConfigOverrides
    // TODO CreatedBy

    UPROPERTY()
    bool bDisabled;

    /// Freeze or unfreeze the channel
    UPROPERTY()
    bool bFrozen;

    // TODO it seems this should really be a ChannelMemberRequest
    UPROPERTY()
    TArray<FString> Members;

    // TODO OwnCapabilities

    /// Team the channel belongs to (if multi-tenant mode is enabled)
    UPROPERTY()
    FString Team;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
