// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "BanResponseDto.h"
#include "CoreMinimal.h"
#include "Response/ResponseDto.h"

#include "QueryBannedUsersResponseDto.generated.h"

/**
 * @brief #/components/schemas/QueryBannedUsersResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FQueryBannedUsersResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FBanResponseDto> Bans;
};
