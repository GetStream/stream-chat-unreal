// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChatEvent.h"
#include "CoreMinimal.h"

#include "ChannelEvent.generated.h"

/**
 * @brief Represents an event that happened in a Stream channel
 * @see https://getstream.io/chat/docs/unreal/event_object/#event-object
 * @ingroup StreamChatWebSocket
 */
USTRUCT()
struct FChannelEvent : public FChatEvent
{
    GENERATED_BODY()

    explicit FChannelEvent() = default;

    explicit FChannelEvent(const FName& Type, const FDateTime& CreatedAt, const FString& ChannelId, const FString& ChannelType, const FString& Cid)
        : FChatEvent(Type, CreatedAt), ChannelId(ChannelId), ChannelType(ChannelType), Cid(Cid)
    {
    }

    UPROPERTY()
    FString ChannelId;

    UPROPERTY()
    FString ChannelType;

    /// Channel CID (<type>:<id>)
    UPROPERTY()
    FString Cid;
};
