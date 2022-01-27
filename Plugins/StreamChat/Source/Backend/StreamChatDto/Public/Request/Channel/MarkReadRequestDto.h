// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"

#include "MarkReadRequestDto.generated.h"

struct FSortParamRequestDto;

/**
 * @brief #/components/schemas/MarkReadRequest
 * @ingroup StreamChatDto
 */
USTRUCT()
struct STREAMCHATDTO_API FMarkReadRequestDto
{
    GENERATED_BODY()

    /// Add a message read Id
    /// ID of the message that is considered last read by client
    void SetMessageId(const FString&);

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
