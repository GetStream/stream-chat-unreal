// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PaginationOptions.generated.h"

/**
 * @brief Options for pagination
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FPaginationOptions
{
    GENERATED_BODY()

    FPaginationOptions(int32 Limit = -1, int32 Offset = -1);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    int32 Limit = -1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    int32 Offset = -1;

    TOptional<uint32> GetLimitAsOptional() const;
    TOptional<uint32> GetOffsetAsOptional() const;

protected:
    uint32 GetLimitForDto() const;
    uint32 GetOffsetForDto() const;
};
