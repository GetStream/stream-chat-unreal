// © Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "StreamChatSettings.generated.h"

/**
 *
 */
UCLASS(Config = Game, DefaultConfig)
class STREAMCHAT_API UStreamChatSettings final : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Stream Chat Settings")
    FString Host = TEXT("chat.stream-io-api.com");

    /// How frequently the backend is pinged to keep the WebSocket connection alive in seconds
    UPROPERTY(EditAnywhere, Category = "Stream Chat Settings|WebSocket")
    float WebSocketKeepAliveInterval = 20.f;

    /// How frequently the WebSocket connection is verified and reconnected if necessary
    UPROPERTY(EditAnywhere, Category = "Stream Chat Settings|WebSocket")
    float WebSocketCheckReconnectInterval = 10.f;

    /// If nothing has been received in the WebSocket for this duration, a reconnection is necessary
    UPROPERTY(EditAnywhere, Category = "Stream Chat Settings|WebSocket")
    float WebSocketReconnectionTimeout = 40.f;
};
