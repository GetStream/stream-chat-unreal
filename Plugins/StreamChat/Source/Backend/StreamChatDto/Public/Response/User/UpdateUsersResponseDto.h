// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/ResponseDto.h"
#include "UserObjectDto.h"

#include "UpdateUsersResponseDto.generated.h"

/**
 * @brief #/components/schemas/UpdateUsersResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FUpdateUsersResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<FString, FUserObjectDto> Users;
};
