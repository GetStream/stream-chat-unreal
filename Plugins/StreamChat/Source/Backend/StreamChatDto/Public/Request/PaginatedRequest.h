// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"

#include "PaginatedRequest.generated.h"

USTRUCT()
struct STREAMCHATDTO_API FPaginatedRequest
{
    GENERATED_BODY()

    /// Number of items to return
    UPROPERTY()
    uint32 Limit = TNumericLimits<uint32>::Max();

    /// Item offset
    UPROPERTY()
    uint32 Offset = TNumericLimits<uint32>::Max();

    UPROPERTY()
    FJsonObjectWrapper FilterConditions;
};
