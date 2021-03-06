// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "UserResponseDto.generated.h"

/**
 * @brief #/components/schemas/UserResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUserResponseDto : public FUserObjectDto
{
    GENERATED_BODY()

    /// Whether user is shadow banned or not
    UPROPERTY()
    bool bShadowBanned = false;
};
