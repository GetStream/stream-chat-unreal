// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event/Abstract/ChannelEvent.h"
#include "UserObjectDto.h"

#include "TypingStartEvent.generated.h"

/**
 * @brief A user started typing
 * @see https://getstream.io/chat/docs/events/#typing.start
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FTypingStartEvent : public FChannelEvent
{
    GENERATED_BODY()

    explicit FTypingStartEvent() = default;
    explicit FTypingStartEvent(
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
        static const FName Type{TEXT("typing.start")};
        return Type;
    }

    UPROPERTY()
    FString ParentId;

    UPROPERTY()
    FUserObjectDto User;
};
