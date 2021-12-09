// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MessageDto.h"
#include "Response/Channel/ChannelResponseDto.h"

#include "SearchResultMessageDto.generated.h"

/**
 * @brief #/components/schemas/SearchResultMessage
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FSearchResultMessageDto : public FMessageDto
{
    GENERATED_BODY()

    UPROPERTY()
    FChannelResponseDto Channel;
};
