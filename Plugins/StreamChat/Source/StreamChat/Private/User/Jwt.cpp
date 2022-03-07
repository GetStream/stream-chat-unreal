// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/Jwt.h"

#include "Misc/Base64.h"

FString Jwt::Development(const FString& UserId)
{
    const FString Header = TEXT("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9");    //  {"alg": "HS256", "typ": "JWT"}
    const FString DevSignature = TEXT("devtoken");
    const FString Json = FString::Printf(TEXT("{\"user_id\":\"%s\"}"), *UserId);
    const FString Payload = FBase64::Encode(Json);
    return FString::Printf(TEXT("%s.%s.%s"), *Header, *Payload, *DevSignature);
}
