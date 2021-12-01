// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// © Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "StreamChatWebSocketSettings.generated.h"

/**
 * @brief Project Settings for Stream Chat WebSockets
 * @ingroup StreamChatWebSocket
 */
UCLASS(Config = Game, DefaultConfig)
class STREAMCHATWEBSOCKET_API UStreamChatWebSocketSettings final : public UObject
{
    GENERATED_BODY()

public:
    /// How frequently the backend is pinged to keep the WebSocket connection alive in seconds
    UPROPERTY(EditAnywhere, Category = "WebSocket")
    float KeepAliveInterval = 20.f;

    /// How frequently the WebSocket connection is verified and reconnected if necessary
    UPROPERTY(EditAnywhere, Category = "WebSocket")
    float CheckReconnectInterval = 10.f;

    /// If nothing has been received in the WebSocket for this duration, a reconnection is necessary
    UPROPERTY(EditAnywhere, Category = "WebSocket")
    float ReconnectionTimeout = 40.f;
};
