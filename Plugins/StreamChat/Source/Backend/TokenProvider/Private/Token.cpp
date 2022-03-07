// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Token.h"

FToken FToken::Jwt(const FString& InJwtString)
{
    return FToken{ETokenType::Jwt, InJwtString};
}

FToken FToken::Anonymous()
{
    return FToken{ETokenType::Anonymous};
}
