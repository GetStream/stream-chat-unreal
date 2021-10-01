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
	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "API Key"))
	FString ApiKey;
};
