// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/ResponseDto.h"
#include "UserResponseDto.h"

#include "UsersResponseDto.generated.h"

/**
 * @brief #/components/schemas/UsersResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUsersResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// List of found users
    UPROPERTY()
    TArray<FUserResponseDto> Users;
};
