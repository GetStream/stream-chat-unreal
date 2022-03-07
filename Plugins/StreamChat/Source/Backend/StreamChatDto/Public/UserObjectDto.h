// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserDto.h"

#include "UserObjectDto.generated.h"

/**
 * @brief #/components/schemas/UserObject
 * Used in both requests and responses
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FUserObjectDto : public FUserDto
{
    GENERATED_BODY()

    explicit FUserObjectDto() = default;
    explicit FUserObjectDto(
        const FUserDto& UserDto,
        const FDateTime& CreatedAt = FDateTime{0},
        const FDateTime& UpdatedAt = FDateTime{0},
        const FDateTime& DeactivatedAt = FDateTime{0},
        const FDateTime& DeletedAt = FDateTime{0},
        const FDateTime& LastActive = FDateTime{0},
        const bool bOnline = false)
        : FUserDto{UserDto}
        , CreatedAt{CreatedAt}
        , UpdatedAt{UpdatedAt}
        , DeactivatedAt{DeactivatedAt}
        , DeletedAt{DeletedAt}
        , LastActive{LastActive}
        , bOnline(bOnline)
    {
    }

    /// Date/time of creation
    UPROPERTY(Transient)
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time of the last update
    UPROPERTY(Transient)
    FDateTime UpdatedAt = FDateTime{0};

    /// Date/time of deactivation
    UPROPERTY(Transient)
    FDateTime DeactivatedAt = FDateTime{0};

    /// Date/time of deletion
    UPROPERTY(Transient)
    FDateTime DeletedAt = FDateTime{0};

    /// Date of last activity
    UPROPERTY(Transient)
    FDateTime LastActive = FDateTime{0};

    /// Whether a user is online or not
    UPROPERTY(Transient)
    bool bOnline = false;
};
