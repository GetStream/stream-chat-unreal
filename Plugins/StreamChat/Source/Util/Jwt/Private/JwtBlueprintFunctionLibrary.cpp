// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "JwtBlueprintFunctionLibrary.h"

#include "Jwt.h"

FString UJwtBlueprintFunctionLibrary::GenerateJwt(const FString& ApiSecret, const FString& UserId)
{
    return Jwt::GenerateJwt(ApiSecret, UserId);
}
