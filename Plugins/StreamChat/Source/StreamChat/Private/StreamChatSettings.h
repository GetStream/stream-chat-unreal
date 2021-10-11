// © Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "StreamChatSettings.generated.h"

/**
 *
 */
UCLASS(Config = Game, DefaultConfig)
class UStreamChatSettings final : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    FString Host = TEXT("chat.stream-io-api.com");

    /// How frequently the backend is pinged to keep the WebSocket connection alive in seconds
    UPROPERTY(EditAnywhere)
    float WebSocketKeepAliveInterval = 20.f;
};
