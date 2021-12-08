// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "JsonPlaceholderPost.generated.h"

USTRUCT()
struct FJsonPlaceholderPost
{
    GENERATED_BODY()

    UPROPERTY()
    int32 User = -1;

    UPROPERTY()
    int32 Id = -1;

    UPROPERTY()
    FString Title;

    UPROPERTY()
    FString Body;
};
