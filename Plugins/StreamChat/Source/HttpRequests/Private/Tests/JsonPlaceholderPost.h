#pragma once

#include "CoreMinimal.h"

#include "JsonPlaceholderPost.generated.h"

USTRUCT()
struct FJsonPlaceholderPost
{
    GENERATED_BODY()

    UPROPERTY()
    int32 UserId;

    UPROPERTY()
    int32 Id;

    UPROPERTY()
    FString Title;

    UPROPERTY()
    FString Body;
};
