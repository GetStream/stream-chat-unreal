﻿// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UserObjectDto.generated.h"

/**
 * #/components/schemas/UserObject
 */
USTRUCT(BlueprintType)
struct STREAMCHATDTO_API FUserObjectDto
{
    GENERATED_BODY()

    UPROPERTY()
    FString Id;

    // TODO rest of the fields
};