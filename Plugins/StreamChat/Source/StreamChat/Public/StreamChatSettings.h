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
    UPROPERTY(EditAnywhere, Category = "General")
    FString Host = TEXT("chat.stream-io-api.com");
};
