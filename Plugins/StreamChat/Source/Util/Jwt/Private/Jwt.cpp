#include "Jwt.h"

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
