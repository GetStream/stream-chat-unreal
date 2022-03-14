// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "ReadDto.generated.h"

/**
 * @brief #/components/schemas/Read
 * @ingroup StreamChatDto
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FReadDto
{
    GENERATED_BODY()

    UPROPERTY()
    FDateTime LastRead = FDateTime{0};

    UPROPERTY()
    uint32 UnreadMessages = 0;

    UPROPERTY()
    FUserObjectDto User;
};
