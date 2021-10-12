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
    UPROPERTY(EditAnywhere, Category = "Stream Chat Settings")
    float WebSocketKeepAliveInterval = 20.f;
};
