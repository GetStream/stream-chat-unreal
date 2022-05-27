// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserObjectRequestDto.h"

#include "UpdateUsersRequestDto.generated.h"

/**
 * @brief #/components/schemas/UpdateUsersRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateUsersRequestDto
{
    GENERATED_BODY()

    /// Object containing users
    UPROPERTY()
    TMap<FString, FUserObjectRequestDto> Users;
};
