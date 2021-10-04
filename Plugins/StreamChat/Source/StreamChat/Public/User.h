// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "User.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUser
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Id;
};
