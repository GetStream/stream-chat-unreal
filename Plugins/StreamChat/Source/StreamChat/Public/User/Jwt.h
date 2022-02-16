// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * @brief Utilities for JSON Web Token manipulation
 */
namespace Jwt
{
/**
 * @brief Generate a JSON Web Token which can be used during development
 * @param UserId The user ID of the user you wish to generate a token for
 * @return A newly generated JWT
 */
STREAMCHAT_API FString Development(const FString& UserId);
}    // namespace Jwt
