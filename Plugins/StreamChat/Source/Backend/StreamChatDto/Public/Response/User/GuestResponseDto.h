// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "GuestResponseDto.generated.h"

/**
 * @brief #/components/schemas/GuestResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FGuestResponseDto
{
    GENERATED_BODY()

    /// Authentication token to use for guest user
    UPROPERTY()
    FString AccessToken;

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    /// Created user object
    UPROPERTY()
    FUserObjectDto User;
};
