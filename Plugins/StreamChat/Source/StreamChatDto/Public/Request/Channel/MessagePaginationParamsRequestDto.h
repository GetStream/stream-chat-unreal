#pragma once

#include "CoreMinimal.h"

#include "MessagePaginationParamsRequestDto.generated.h"

USTRUCT()
struct STREAMCHATDTO_API FMessagePaginationParamsRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    FDateTime CreatedAtAfter;

    UPROPERTY()
    FDateTime CreatedAtAfterOrEqual;

    UPROPERTY()
    FDateTime CreatedAtBefore;

    UPROPERTY()
    FDateTime CreatedAtBeforeOrEqual;

    UPROPERTY()
    FString IdGt;

    UPROPERTY()
    FString IdGte;

    UPROPERTY()
    FString IdLt;

    UPROPERTY()
    FString IdLte;

    UPROPERTY()
    uint32 Limit;

    UPROPERTY()
    int32 Offset;
};
