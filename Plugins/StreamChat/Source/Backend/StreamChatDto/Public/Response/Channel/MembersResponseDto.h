// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelStateResponseFieldsDto.h"
#include "CoreMinimal.h"
#include "Response/ResponseDto.h"

#include "MembersResponseDto.generated.h"

/**
 * @brief #/components/schemas/MembersResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FMembersResponseDto : public FResponseDto
{
    GENERATED_BODY()

    /// List of channels
    UPROPERTY()
    TArray<FChannelMemberDto> Members;
};
