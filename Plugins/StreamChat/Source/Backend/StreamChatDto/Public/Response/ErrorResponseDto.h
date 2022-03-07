// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ErrorResponseDto.generated.h"

/**
 * @brief #/components/schemas/APIError
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FErrorResponseDto
{
    GENERATED_BODY()

    bool IsTokenExpired() const;

    /// Response HTTP status code
    UPROPERTY()
    int32 StatusCode = -1;

    /// API error code
    UPROPERTY()
    int32 Code = -1;

    /// Request duration
    UPROPERTY()
    FString Duration;

    /// Message describing an error
    UPROPERTY()
    FString Message;

    /// URL with additional information
    UPROPERTY()
    FString MoreInfo;
};
