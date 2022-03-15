// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BanPaginationOptions.generated.h"

/**
 * @brief Options for pagination of bans
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FBanPaginationOptions
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    int32 Limit = 20;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    int32 Offset = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtAfterOrEqual;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtAfter;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtBeforeOrEqual;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtBefore;
};
