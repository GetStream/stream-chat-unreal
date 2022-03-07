// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserDto.h"

#include "UserObjectRequestDto.generated.h"

/**
 * @brief #/components/schemas/UserObjectRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUserObjectRequestDto : public FUserDto
{
    GENERATED_BODY()
    explicit FUserObjectRequestDto() = default;

    explicit FUserObjectRequestDto(const FUserDto& UserDto) : FUserDto{UserDto}
    {
        ;
    };
};
