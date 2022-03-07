// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserObjectRequestDto.h"

#include "GuestRequestDto.generated.h"

/**
 * @brief #/components/schemas/GuestRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FGuestRequestDto
{
    GENERATED_BODY()

    UPROPERTY()
    FUserObjectRequestDto User;
};
