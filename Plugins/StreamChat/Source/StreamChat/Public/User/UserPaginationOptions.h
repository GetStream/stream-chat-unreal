// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Request/Channel/PaginationParamsRequestDto.h"

#include "UserPaginationOptions.generated.h"

/**
 * @brief Options for pagination of members or watchers
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FUserPaginationOptions
{
    GENERATED_BODY()
    explicit operator FPaginationParamsRequestDto() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    int32 Limit = 20;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    int32 Offset = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FString IdGt;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FString IdGte;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FString IdLt;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FString IdLte;
};
