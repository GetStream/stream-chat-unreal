#pragma once

#include "CoreMinimal.h"

/**
 * @brief Utilities for JSON Web Token manipulation
 */
namespace Jwt
{
/**
 * @brief Generate a JSON Web Token
 * @param ApiSecret Your API secret as listed on the Dashboard
 * @param UserId The user ID of the user you wish to generate a token for
 * @return A newly generated JWT
 */
FString GenerateJwt(const FString& ApiSecret, const FString& UserId);
}    // namespace Jwt
