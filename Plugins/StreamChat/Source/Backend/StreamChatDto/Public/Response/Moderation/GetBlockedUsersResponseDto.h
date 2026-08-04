// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Response/User/UserResponseDto.h"

#include "GetBlockedUsersResponseDto.generated.h"

/**
 * @brief #/components/schemas/BlockedUserResponse
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FBlockedUserDto
{
    GENERATED_BODY()

    /// Id of the user who created the block
    UPROPERTY()
    FString UserId;

    /// The user who created the block
    UPROPERTY()
    FUserResponseDto User;

    /// Id of the user who was blocked
    UPROPERTY()
    FString BlockedUserId;

    /// The user who was blocked
    UPROPERTY()
    FUserResponseDto BlockedUser;

    /// When the block was created
    UPROPERTY()
    FDateTime CreatedAt = FDateTime{0};
};

/**
 * @brief #/components/schemas/GetBlockedUsersResponse
 * @ingroup StreamChatDto
 * @see https://getstream.io/chat/docs/unreal/moderation/
 */
USTRUCT()
struct STREAMCHATDTO_API FGetBlockedUsersResponseDto
{
    GENERATED_BODY()

    /// Duration of the request in human-readable format
    UPROPERTY()
    FString Duration;

    /// The blocks created by the current user
    UPROPERTY()
    TArray<FBlockedUserDto> Blocks;
};
