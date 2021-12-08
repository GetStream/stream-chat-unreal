// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"

#include "MessagePaginationParamsRequestDto.generated.h"

class FJsonObject;

/**
 * @brief #/components/schemas/MessagePaginationParamsRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FMessagePaginationParamsRequestDto
{
    GENERATED_BODY()

    void SetCreatedAtAfter(const FDateTime&);

    void SetCreatedAtAfterOrEqual(const FDateTime&);

    void SetCreatedAtBefore(const FDateTime&);

    void SetCreatedAtBeforeOrEqual(const FDateTime&);

    void SetIdGt(const FString&);

    void SetIdGte(const FString&);

    void SetIdLt(const FString&);

    void SetIdLte(const FString&);

    UPROPERTY()
    uint32 Limit = 20;

    UPROPERTY()
    int32 Offset = 0;

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
