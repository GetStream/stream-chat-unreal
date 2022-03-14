// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "TypingStopEvent.generated.h"

/**
 * @brief A user stoped typing
 * @see https://getstream.io/chat/docs/events/#typing.stop
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FTypingStopEvent : public FChannelEvent
{
    GENERATED_BODY()
    explicit FTypingStopEvent() = default;

    FTypingStopEvent(
        const FDateTime& CreatedAt,
        const FString& ChannelId,
        const FString& ChannelType,
        const FString& Cid,
        const FString& ParentId,
        const FUserObjectDto& User)
        : FChannelEvent(StaticType(), CreatedAt, ChannelId, ChannelType, Cid), ParentId(ParentId), User(User)
    {
    }

    static FName StaticType()
    {
        static const FName Type{TEXT("typing.stop")};
        return Type;
    }

    UPROPERTY()
    FString ParentId;

    UPROPERTY()
    FUserObjectDto User;
};
