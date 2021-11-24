// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SortOption.generated.h"

struct FSortParamRequestDto;

/**
 * @brief The desired sort direction for Stream Chat API queries
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class ESortDirection : uint8
{
    Ascending,
    Descending
};

/**
 * @brief The desired sort field for Stream Chat API queries
 * @ingroup StreamChat
 */
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

/**
 * @brief The desired sort options for Stream Chat API queries
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FSortOption
{
    GENERATED_BODY()

    explicit operator FSortParamRequestDto() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    ESortField Field;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat")
    ESortDirection Direction;
};
