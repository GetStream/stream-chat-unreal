// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReactionDto.h"
#include "Response/Message/MessageDto.h"

#include "ReactionResponseDto.generated.h"

/**
 * @brief #/components/schemas/ReactionResponse
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FReactionResponseDto
{
    GENERATED_BODY()

    STREAMCHATDTO_API static void DeserializeExtra(const FJsonObject&, FReactionResponseDto&);

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    UPROPERTY()
    FMessageDto Message;

    UPROPERTY()
    FReactionDto Reaction;
};
