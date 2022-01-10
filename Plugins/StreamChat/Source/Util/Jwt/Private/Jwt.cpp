// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Jwt.h"

#include "Misc/Base64.h"

#define UI UI_UE
#pragma push_macro("verify")
#undef verify
THIRD_PARTY_INCLUDES_START
#include "jwt-cpp/jwt.h"
THIRD_PARTY_INCLUDES_END
#pragma pop_macro("verify")
#undef UI

FString Jwt::GenerateJwt(const FString& ApiSecret, const FString& UserId)
{
    const auto Secret = std::string{TCHAR_TO_UTF8(*ApiSecret)};
    const auto Id = std::string{TCHAR_TO_UTF8(*UserId)};
    const auto Token = jwt::create().set_type("JWT").set_payload_claim("user_id", jwt::claim{Id}).sign(jwt::algorithm::hs256{Secret});
    return UTF8_TO_TCHAR(Token.c_str());
}

FString Jwt::Development(const FString& UserId)
{
    const FString Header = TEXT("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9");    //  {"alg": "HS256", "typ": "JWT"}
    const FString DevSignature = TEXT("devtoken");
    const FString Json = FString::Printf(TEXT("{\"user_id\":\"%s\"}"), *UserId);
    const FString Payload = FBase64::Encode(Json);
    return FString::Printf(TEXT("%s.%s.%s"), *Header, *Payload, *DevSignature);
}
