// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BanRequestDto.generated.h"

/**
 * @brief #/components/schemas/BanRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FBanRequestDto
{
    GENERATED_BODY()

    /// ID of user to ban
    UPROPERTY()
    FString TargetUserId;

    /// Channel type to ban user in (not serialized if empty)
    UPROPERTY()
    FString Type;

    /// Channel ID to ban user in (not serialized if empty)
    UPROPERTY()
    FString Id;

    /// Ban reason (not serialized if empty)
    UPROPERTY()
    FString Reason;

    /// Whether to perform shadow ban or not
    UPROPERTY()
    bool bShadow = false;

    /// Whether to perform IP ban or not
    UPROPERTY()
    bool bIpBan = false;

    /// Timeout of ban in minutes. User will be unbanned after this period of time
    UPROPERTY()
    float Timeout = TNumericLimits<float>::Max();
};
