// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UpdateUserPartialRequestDto.h"

#include "UpdateUsersPartialRequestDto.generated.h"

/**
 * @brief #/components/schemas/UpdateUsersPartialRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateUsersPartialRequestDto
{
    GENERATED_BODY()

    /// User ID to update
    UPROPERTY()
    TArray<FUpdateUserPartialRequestDto> Users;
};
