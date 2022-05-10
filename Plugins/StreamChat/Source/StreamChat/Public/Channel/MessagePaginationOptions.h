// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Request/Channel/MessagePaginationParamsRequestDto.h"

#include "MessagePaginationOptions.generated.h"

/**
 * @brief Options for pagination of channels
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FMessagePaginationOptions
{
    GENERATED_BODY()
    explicit operator FMessagePaginationParamsRequestDto() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    int32 Limit = 20;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    int32 Offset = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtAfter = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtAfterOrEqual = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtBefore = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtBeforeOrEqual = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FString IdGt;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FString IdGte;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FString IdLt;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FString IdLte;
};
