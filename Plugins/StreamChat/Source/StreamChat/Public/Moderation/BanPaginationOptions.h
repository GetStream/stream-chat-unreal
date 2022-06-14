// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaginationOptions.h"

#include "BanPaginationOptions.generated.h"

/**
 * @brief Options for pagination of bans
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FBanPaginationOptions : public FPaginationOptions
{
    GENERATED_BODY()

    FBanPaginationOptions(int32 Limit = -1, int32 Offset = -1);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtAfter = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtAfterOrEqual = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtBefore = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtBeforeOrEqual = FDateTime{0};
};
