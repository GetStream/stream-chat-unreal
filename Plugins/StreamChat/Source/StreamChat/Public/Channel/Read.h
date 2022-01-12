// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "User/UserRef.h"

#include "Read.generated.h"

class FUserManager;
struct FReadDto;

/**
 * @brief Contains information about the read state (i.e number of unread messages) of a member of a chat channel.
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct FRead
{
    GENERATED_BODY()

    /// Needed by USTRUCT system. Shouldn't be used directly.
    FRead();
    /// Create a read state from a DTO from the API
    explicit FRead(UUserManager&, const FReadDto&);

    /// The user to which this read state applies
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Read")
    FUserRef User;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Read")
    int32 UnreadMessages = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stream Chat|Channel|Read")
    FDateTime LastRead = FDateTime{0};
};
