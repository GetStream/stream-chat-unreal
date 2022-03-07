// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserResponseDto.h"

#include "UsersResponseDto.generated.h"

/**
 * @brief #/components/schemas/UsersResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUsersResponseDto
{
    GENERATED_BODY()

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    /// List of found users
    UPROPERTY()
    TArray<FUserResponseDto> Users;
};
