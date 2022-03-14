// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/ResponseDto.h"
#include "UserObjectDto.h"

#include "GuestResponseDto.generated.h"

/**
 * @brief #/components/schemas/GuestResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FGuestResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// Authentication token to use for guest user
    UPROPERTY()
    FString AccessToken;

    /// Created user object
    UPROPERTY()
    FUserObjectDto User;
};
