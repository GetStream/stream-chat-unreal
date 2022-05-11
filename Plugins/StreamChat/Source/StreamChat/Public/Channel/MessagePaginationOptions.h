// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Request/Channel/MessagePaginationParamsRequestDto.h"
#include "User/UserPaginationOptions.h"

#include "MessagePaginationOptions.generated.h"

/**
 * @brief Options for pagination of messages
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FMessagePaginationOptions : public FUserPaginationOptions
{
    GENERATED_BODY()
    explicit operator FMessagePaginationParamsRequestDto() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtAfter = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtAfterOrEqual = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtBefore = FDateTime{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stream Chat")
    FDateTime CreatedAtBeforeOrEqual = FDateTime{0};
};
