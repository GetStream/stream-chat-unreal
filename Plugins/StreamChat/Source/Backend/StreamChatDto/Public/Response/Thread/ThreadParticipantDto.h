// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AdditionalFields.h"
#include "CoreMinimal.h"
#include "UserObjectDto.h"

#include "ThreadParticipantDto.generated.h"

/**
 * @brief #/components/schemas/ThreadParticipant
 * Represents a user that is participating in a thread.
 * Used in responses only
 * @ingroup StreamChatDto
 */
USTRUCT()
struct FThreadParticipantDto
{
    GENERATED_BODY()

    /// CID of the channel the thread belongs to
    UPROPERTY()
    FString ChannelCid;

    /// Unique identifier of the thread, which is the ID of its parent message
    UPROPERTY()
    FString ThreadId;

    /// Unique identifier of the participating user
    UPROPERTY()
    FString UserId;

    /// The participating user
    UPROPERTY()
    FUserObjectDto User;

    /// Date/time the user joined the thread
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};

    /// Date/time the user last read this thread
    UPROPERTY()
    FDateTime LastReadAt = FDateTime{0};

    /// Date/time of the last message this user sent in the thread
    UPROPERTY()
    FDateTime LastThreadMessageAt = FDateTime{0};

    /// Date/time the user left the thread
    UPROPERTY()
    FDateTime LeftThreadAt = FDateTime{0};

    UPROPERTY()
    FAdditionalFields AdditionalFields;
};
