// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"

#include "QueryBannedUsersRequestDto.generated.h"

/**
 * @brief #/components/schemas/QueryBannedUsersRequest
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FQueryBannedUsersRequestDto
{
    GENERATED_BODY()

    /// Timeout of ban in minutes. User will be unbanned after this period of time
    void SetTimeout(float);

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

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
