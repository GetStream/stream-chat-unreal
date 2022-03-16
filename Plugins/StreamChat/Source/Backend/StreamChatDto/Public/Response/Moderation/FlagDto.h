// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "FlagDto.generated.h"

/**
 * @brief #/components/schemas/Flag
 * Contains information about flagged user or message
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FFlagDto
{
    GENERATED_BODY()

    /// Date of the approval
    UPROPERTY()
    FDateTime ApprovedAt = FDateTime{0};

    /// Date/time of creation
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    UPROPERTY()
    bool bCreatedByAutomod = false;

    UPROPERTY()
    FAdditionalFields Details;

    /// Date of the rejection
    UPROPERTY()
    FDateTime RejectedAt = FDateTime{0};

    /// Date of the review
    UPROPERTY()
    FDateTime ReviewedAt = FDateTime{0};

    // Id of flagged message
    UPROPERTY()
    FString TargetMessageId;

    // Flagged user
    UPROPERTY()
    FUserObjectDto TargetUser;

    /// Date/time of the last update
    UPROPERTY()
    FDateTime UpdatedAt = FDateTime{0};

    // User who flagged a message or a user
    UPROPERTY()
    FUserObjectDto User;
};
