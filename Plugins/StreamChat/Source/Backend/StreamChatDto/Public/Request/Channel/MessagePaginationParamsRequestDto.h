// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MessagePaginationParamsRequestDto.generated.h"

class FJsonObject;

/**
 * @brief #/components/schemas/MessagePaginationParamsRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMessagePaginationParamsRequestDto
{
    GENERATED_BODY()

    STREAMCHATDTO_API static void SerializeExtra(const FMessagePaginationParamsRequestDto&, FJsonObject&);

    TOptional<FDateTime> CreatedAtAfter;

    TOptional<FDateTime> CreatedAtAfterOrEqual;

    TOptional<FDateTime> CreatedAtBefore;

    TOptional<FDateTime> CreatedAtBeforeOrEqual;

    TOptional<FString> IdGt;

    TOptional<FString> IdGte;

    TOptional<FString> IdLt;

    TOptional<FString> IdLte;

    UPROPERTY()
    uint32 Limit = 20;

    UPROPERTY()
    int32 Offset = 0;
};
