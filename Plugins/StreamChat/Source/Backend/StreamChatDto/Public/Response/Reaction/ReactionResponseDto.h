// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReactionDto.h"
#include "Response/Message/MessageDto.h"
#include "Response/ResponseDto.h"

#include "ReactionResponseDto.generated.h"

/**
 * @brief #/components/schemas/ReactionResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FReactionResponseDto : public FResponseDto
{
    GENERATED_BODY()

    UPROPERTY()
    FMessageDto Message;

    UPROPERTY()
    FReactionDto Reaction;
};
