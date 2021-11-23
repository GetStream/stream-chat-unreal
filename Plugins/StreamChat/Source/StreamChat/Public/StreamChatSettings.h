// Copyright Stream.IO, Inc. All Rights Reserved.

// © Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "StreamChatSettings.generated.h"

/**
 * @brief Project Settings for Stream Chat
 * @ingroup StreamChat
 */
UCLASS(Config = Game, DefaultConfig)
class STREAMCHAT_API UStreamChatSettings final : public UObject
{
    GENERATED_BODY()

public:
    /// The hostname to connect to via https and WebSockets
    UPROPERTY(EditAnywhere, Category = "General")
    FString Host = TEXT("chat.stream-io-api.com");
};
