// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "UserBannedEvent.generated.h"

/**
 * @brief #/components/schemas/UserBannedEvent
 * @see https://getstream.io/chat/docs/events/#user.banned
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FUserBannedEvent : public FChannelEvent
{
    GENERATED_BODY()

    static FName StaticType()
    {
        static const FName Type{TEXT("user.banned")};
        return Type;
    }

    UPROPERTY()
    FDateTime Expiration = FDateTime{0};
    UPROPERTY()
    FString Reason;
    UPROPERTY()
    bool bShadow = false;
    UPROPERTY()
    FUserObjectDto CreatedBy;
    UPROPERTY()
    FUserObjectDto User;
};
