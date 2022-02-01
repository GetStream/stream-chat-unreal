// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Token.generated.h"

/**
 * @brief The type of token
 * @ingroup StreamChat
 */
UENUM(BlueprintType)
enum class ETokenType : uint8
{
    /// JSON Web Token
    Jwt,
    /// Very restricted user type
    Anonymous,
};

USTRUCT(BlueprintType)
struct TOKENPROVIDER_API FToken
{
    GENERATED_BODY()

public:
    static FToken Jwt(const FString& InJwtString);
    static FToken Anonymous();

    ETokenType TokenType = ETokenType::Anonymous;
    FString JwtString;
};
