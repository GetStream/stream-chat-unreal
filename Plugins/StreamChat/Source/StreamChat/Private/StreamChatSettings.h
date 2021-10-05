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
	FString Host = TEXT("chat-us-east-1.stream-io-api.com");
};
