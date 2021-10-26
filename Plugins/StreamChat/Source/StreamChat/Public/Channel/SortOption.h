#pragma once

#include "CoreMinimal.h"

#include "SortOption.generated.h"

struct FSortParamRequestDto;

UENUM(BlueprintType)
enum class ESortDirection : uint8
{
    Ascending,
    Descending
};

UENUM(BlueprintType)
enum class ESortField : uint8
{
    LastUpdated,
    LastMessageAt,
    UpdatedAt,
    CreatedAt,
    MemberCount,
    UnreadCount,
    HasUnread
};

USTRUCT(BlueprintType)
struct FSortOption
{
    GENERATED_BODY()

    explicit operator FSortParamRequestDto() const;

    UPROPERTY()
    ESortField Field;

    UPROPERTY()
    ESortDirection Direction;
};
